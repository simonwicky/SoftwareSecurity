#include "inc/services/threadpool.hpp"
#include "inc/commands/command.hpp"
#include "inc/server/server.hpp"
#include "inc/user/user.hpp"
#include "inc/services/logging.hpp"
#include "inc/grass.h"
#include "cstdio"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// instance definitions
Server::Server(Socket *socket, CfgFileParser *pParser) {
  this->socket = socket;
  this->workerThread = nullptr;
  this->configParser = pParser;
}

void Server::startServer() {
  workerThread = new std::thread(loop);
}

Server::~Server() {
  finish();
  delete (workerThread);
}

void Server::finish() {
  if (!this->exit) {
    socket->shutdownSocket();
    server->socket->closeSocket();
    LOG_INFO("Starting finish");
    this->exit = true;
    LOG_INFO("Waiting for main loop");
  }
  LOG_INFO("Joined main server loop");
}

void Server::wait() {
  workerThread->join();
}

// class instance definitions

Server *Server::server = nullptr;
bool Server::signalExit = false;
std::mutex Server::serverCreationMutex;

void Server::loop() {
  int opt = 1;
  struct timeval tv{};
  tv.tv_sec = 60;
  tv.tv_usec = 0;
  server->socket->setSocketOption(Socket::Level::SOCKET,
                                  std::vector<Socket::Option>{Socket::Option::REUSEADDR, Socket::Option::REUSEPORT},
                                  &opt, sizeof(opt));

  server->socket->setSocketOption(Socket::Level::SOCKET, std::vector<Socket::Option>{Socket::Option::RCV_TIMEOUT}, &tv,
                                  sizeof(tv));

  SocketAddressIn address;
  address.sin_family = static_cast<sa_family_t>(Socket::Domain::IPv4);
  address.sin_addr.s_addr = INADDR_ANY;

  int port = server->configParser->getPort();

  address.sin_port = htons(port);

  server->socket->socketBind((SocketAddress *) &address, sizeof(address));
  server->socket->socketListen(100);

  LOG_INFO("Creating user dir");
  createUsersDir();

  while (!server->exit) {
    auto newUserAddress = new SocketAddressIn;
    int addressLen = sizeof(*newUserAddress);

    LOG_INFO("Server accepting a connection");
    Socket *newSocket = server->socket->acceptConnection(reinterpret_cast<SocketAddress *>(newUserAddress),
                                                         reinterpret_cast<socketlen *>(&addressLen));

    LOG_INFO("New socket created");
    if (newSocket == nullptr)
      continue;


    if (server->exit) {
      LOG_INFO("Main server loop exited");
      break;
    }

    LOG_INFO(std::string("New connection from: ") + std::to_string(address.sin_addr.s_addr));

    ThreadPool::run([newSocket, newUserAddress] {
      connectedUser(newSocket, newUserAddress);
    });

  }
  rmUsersDir();

  LOG_INFO("Main server loop is exiting");
}


void Server::connectedUser(Socket *socket, SocketAddressIn *userAddressObject) {
  LOG_INFO("Server user connection workerThread");
  std::unique_ptr<Socket> socketWrapper(socket);
  std::unique_ptr<SocketAddressIn, socketAddressInDeleter> addressWrapper(userAddressObject);
  CommandParser commandParser;
  std::string dataToSend;
  std::string recvBuffer;

  LOG_INFO("Reading a config file");
  UserState userState(*server->configParser, userAddressObject->sin_port);

  while (!Server::server->exit) {


    while (!commandParser.hasCommand()) {
      auto result = socketWrapper->socketReceive(recvBuffer, std::vector<Socket::MessageOption>());
      if (server->exit)
        return;
      if (result < 0) {
        LOG_WARN("Client non clean exit");
        return;
      }
      commandParser.addBytes(std::string(recvBuffer));
    }

    auto *command = commandParser.getCommand();
    dataToSend.clear();
    recvBuffer.clear();

    LOG_INFO("Executing a command");
    command->exec(dataToSend, userState);
    check_corrupt_global();
    LOG_INFO("Command executed");
    LOG_INFO("Data to send:" + dataToSend);
    if (userState.exit)
      break;

    if (dataToSend.empty())
      dataToSend.push_back('\n');
    socketWrapper->socketSend(dataToSend, std::vector<Socket::MessageOption>());
  }
  LOG_INFO("User exited");
}

Server *Server::makeServer(Socket *socket, CfgFileParser *pParser) {
  std::lock_guard<std::mutex> guard(serverCreationMutex);
  if (signalExit)
    return nullptr;
  if (server == nullptr) {
    server = new Server(socket, pParser);
    return server;
  } else
    return server;
}

Server *Server::getServer() {
  return server;
}

void Server::putFile(const string filename, int fileSize, volatile int *putHandle) {
  int opt = 1;
  Socket fileRecvSocket(Socket::Domain::IPv4, Socket::CommunicationType::TCP);

  fileRecvSocket.setSocketOption(Socket::Level::SOCKET,
                                 std::vector<Socket::Option>{Socket::Option::REUSEADDR, Socket::Option::REUSEPORT},
                                 &opt, sizeof(opt));


  SocketAddressIn address;
  address.sin_family = static_cast<sa_family_t>(Socket::Domain::IPv4);
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(0);
  fileRecvSocket.socketBind((SocketAddress *) &address, sizeof(address));
  fileRecvSocket.socketListen(100);
  *putHandle = fileRecvSocket.portNumber();

  SocketAddressIn newUserAddress;
  int addressLen = sizeof(SocketAddressIn);
  std::unique_ptr<Socket> recvSocket(fileRecvSocket.acceptConnection(reinterpret_cast<SocketAddress *>(&newUserAddress),
                                                                     reinterpret_cast<socketlen *>(&addressLen)));

  int outputFile = open(filename.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH, S_IWOTH);
  if (outputFile == -1)
    return;

  std::string recvBuffer;
  for (int read = 0, recvCount = 0;
       recvCount < fileSize && !server->exit; recvBuffer.clear(), recvCount += read, read = 0) {
    while (read == 0) {
      read = recvSocket->socketReceive(recvBuffer, std::vector<Socket::MessageOption>());
      if (read == -1 || server->exit)
        break;
    }
    write(outputFile, recvBuffer.c_str(), read);
    recvCount += read;
    read = 0;
    char err_msg[1024];
    snprintf(err_msg, 1024, "FD is %d, input is %s", outputFile, recvBuffer.c_str());
    LOG_ERROR(err_msg);
  }
  close(outputFile);
}

void Server::getFile(const string filename, volatile int *getHandle, volatile int *getSize) {
  char buffer[1024];
  int opt = 1;
  Socket fileRecvSocket(Socket::Domain::IPv4, Socket::CommunicationType::TCP);

  fileRecvSocket.setSocketOption(Socket::Level::SOCKET,
                                 std::vector<Socket::Option>{Socket::Option::REUSEADDR, Socket::Option::REUSEPORT},
                                 &opt, sizeof(opt));


  SocketAddressIn address;
  address.sin_family = static_cast<sa_family_t>(Socket::Domain::IPv4);

  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(0);
  fileRecvSocket.socketBind((SocketAddress *) &address, sizeof(address));

  /* Deliberately using C */
  int inputFile = open(filename.c_str(), O_RDONLY);
  if (inputFile == -1)
    return;

  *getSize = lseek(inputFile, 0, SEEK_END);
  *getHandle = fileRecvSocket.portNumber();

  /* Reset pointer */
  if (lseek(inputFile, 0, SEEK_SET) == -1)
    return;

  SocketAddressIn newUserAddress;
  int addressLen = sizeof(SocketAddressIn);

  Socket *socket = nullptr;

  fileRecvSocket.socketListen(100);

  while (socket == nullptr) {
    socket = fileRecvSocket.acceptConnection(reinterpret_cast<SocketAddress *>(&newUserAddress),
                                             reinterpret_cast<socketlen *>(&addressLen));
  }

  std::unique_ptr<Socket> sendSocket(socket);

  int readBytes;
  while (!server->exit) {
    readBytes = read(inputFile, buffer, 2048);
    if (readBytes == 0 || readBytes == -1)
      break;

    std::string sendBuffer(buffer, readBytes);
    auto sent = sendSocket->socketSend(sendBuffer, std::vector<Socket::MessageOption>());
    if (sent <= 0)
      break;
  }

  close(inputFile);
}
