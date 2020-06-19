#include <inc/services/socket.hpp>
#include <inc/server/server.hpp>
#include <csignal>
#include <inc/services/threadpool.hpp>


void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";

  Server *server = Server::getServer();

  server->finish();
}

#pragma GCC diagnostic ignored "-Wunused-parameter"

int main(int argc, char *argv[]) {
  std::string confFilePath = "grass.conf";
  CfgFileParser config(confFilePath);
  Socket socket(Socket::Domain::IPv4, Socket::CommunicationType::TCP);
  ThreadPool::start();
  Server *server = Server::makeServer(&socket, &config);

  std::cout << "Server is made" << std::endl;
  server->startServer();
  std::cout << "Server is running" << std::endl;
  signal(SIGINT, signalHandler);
  std::cout << "Exit handle added, waiting for termination" << std::endl;
  server->wait();
  std::cout << "Server finished, now thread pool" << std::endl;
  ThreadPool::finish();
  std::cout << "Pool finished" << std::endl;
  return 0;
}