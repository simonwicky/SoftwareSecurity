#include <iostream>
#include "inc/services/socket.hpp"
#include "inc/client/client.hpp"


int main(int argc, char *argv[]) {
  std::cout << "Starting client" << std::endl;

  if (argc - 1 < 2 || argc - 1 > 3) {
    std::cout << "Wrong number of parameters:" << argc - 1 << std::endl;
    return 0;
  }
  Socket socket(Socket::Domain::IPv4, Socket::CommunicationType::TCP);

  std::string address(argv[1]);
  std::string port(argv[2]);

  if (argc == 3) {
    Client::userMode(&socket, address, std::stoi(port));
  } else {
    std::string filePath(argv[3]);
    Client::automatedMode(&socket, address, std::stoi(port), filePath);
  }
}