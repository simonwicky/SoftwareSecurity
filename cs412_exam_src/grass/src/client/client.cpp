#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <thread>

#include "inc/client/client.hpp"
#include "inc/commands/util.hpp"
#include "inc/services/socket.hpp"

void getThread(std::string fileName, std::string serverAddress, int port, int size) {
  Socket socket(Socket::Domain::IPv4, Socket::CommunicationType::TCP);

  if (!socket.socketConnect(serverAddress, port))
    return;

  std::ofstream fs(fileName);
  std::string serverReply;
  int totalRead = 0;

  while (true) {
    int read = socket.socketReceive(serverReply, std::vector<Socket::MessageOption>());
    if (read == -1)
      break;

    fs << serverReply;
    serverReply.clear();

    totalRead += read;
    if (totalRead == size)
      break;
  }

  fs.close();
}

void putThread(std::string fileName, std::string serverAddress, int port, int size) {
  Socket socket(Socket::Domain::IPv4, Socket::CommunicationType::TCP);

  if (!socket.socketConnect(serverAddress, port))
    return;

  char buff[1024];

  std::ifstream fs(fileName, std::ifstream::binary);
  int totalWrit = 0;
  while (!fs.eof()) {
    fs.read(buff, 1024);
    socket.socketSend(buff, fs.gcount(), std::vector<Socket::MessageOption>());
    totalWrit += fs.gcount();
    if (totalWrit >= size)
      break;
  }

  fs.close();
}

std::vector<std::thread> backgroundThreads;

void Client::userMode(Socket *socket, std::string &serverAddress, int port) {

  if (!socket->socketConnect(serverAddress, port))
    return;
  std::string clientCommand;
  std::string serverReply;

  while (!(clientCommand == "exit\n")) {
    clientCommand.clear();
    std::cout << "enter command: ";
    getline(std::cin, clientCommand);
    clientCommand.append("\n");
    socket->socketSend(clientCommand, std::vector<Socket::MessageOption>());
    int nRead;
    do {
      /* Possible block if server reply is multiple of MAX_SIZE */
      nRead = socket->socketReceive(serverReply, std::vector<Socket::MessageOption>());
    } while (nRead == MAX_SIZE);

    /* Special handling of get and put */
    if (clientCommand.substr(0, 3) == "get" && serverReply.substr(0, 5) != "Error") {
      int getPort, getSize;
      std::vector<std::string> tokens;
      stringTokenizer(serverReply, tokens);
      getPort = atoi(tokens[2].c_str());
      getSize = atoi(tokens[4].c_str());
      tokens.clear();
      stringTokenizer(clientCommand, tokens);
      /* The pop_back is to remove the '\n' character */
      tokens[1].pop_back();
      backgroundThreads.emplace_back(getThread, tokens[1], serverAddress, getPort, getSize);
    } else if (clientCommand.substr(0, 3) == "put" && serverReply.substr(0, 5) != "Error") {
      int putPort;
      std::vector<std::string> tokens;
      stringTokenizer(serverReply, tokens);
      putPort = atoi(tokens[2].c_str());
      tokens.clear();
      stringTokenizer(clientCommand, tokens);
      /* The pop_back is to remove the '\n' character */
      tokens[2].pop_back();
      backgroundThreads.emplace_back(putThread, tokens[1], serverAddress, putPort, atoi(tokens[2].c_str()));
    }
    std::cout << serverReply;
    serverReply.clear();
  }

  for (auto &backgroundThread : backgroundThreads)
    if (backgroundThread.joinable())
      backgroundThread.join();
}

void Client::automatedMode(Socket *socket, std::string &serverAddress, int port, std::string &filePath) {

  if (!socket->socketConnect(serverAddress, port))
    return;

  std::ifstream fileStream;

  fileStream.open(filePath);

  if (!fileStream.is_open()) {
    std::cout << "File not opened" << std::endl;
    return;
  }


  std::string clientCommand;
  std::string serverReply;

  std::cout << "Automated mode started" << std::endl;

  while (getline(fileStream, clientCommand)) {

    clientCommand.append("\n");
    std::cout << clientCommand << std::flush;
    socket->socketSend(clientCommand, std::vector<Socket::MessageOption>());
    socket->socketReceive(serverReply, std::vector<Socket::MessageOption>());
    std::cout << serverReply << std::flush;

    /* Special handling of get and put */
    if (clientCommand.substr(0, 3) == "get" && serverReply.substr(0, 5) != "Error") {
      int getPort, getSize;
      std::vector<std::string> tokens;
      stringTokenizer(serverReply, tokens);
      getPort = atoi(tokens[2].c_str());
      getSize = atoi(tokens[4].c_str());
      tokens.clear();
      stringTokenizer(clientCommand, tokens);
      /* The pop_back is to remove the '\n' character */
      tokens[1].pop_back();
      backgroundThreads.emplace_back(getThread, tokens[1], serverAddress, getPort, getSize);
    } else if (clientCommand.substr(0, 3) == "put" && serverReply.substr(0, 5) != "Error") {
      int putPort;
      std::vector<std::string> tokens;
      stringTokenizer(serverReply, tokens);
      putPort = atoi(tokens[2].c_str());
      tokens.clear();
      stringTokenizer(clientCommand, tokens);
      /* The pop_back is to remove the '\n' character */
      tokens[2].pop_back();
      backgroundThreads.emplace_back(putThread, tokens[1], serverAddress, putPort, atoi(tokens[2].c_str()));
    }
    clientCommand.clear();
  serverReply.clear();
  }

  fileStream.close();
  for (auto &backgroundThread : backgroundThreads)
    if (backgroundThread.joinable())
      backgroundThread.join();
}
