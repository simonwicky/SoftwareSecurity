#pragma once

#include <inc/services/socket.hpp>

class Client {

public:
  /**
   * @brief mode of operation where a user enters commands from the command line
   * @param socket used for communicating with a server
   * @param address server address
   * @param port port to connect to
   */
  static void userMode(Socket *socket, std::string &address, int port);

  /**
   * @brief mode of operation where commands read from a file, not used since the project description changed
   * @param socket used to communicate with a server
   * @param address of the server
   * @param port of the server
   * @param filePath to the file containing commands to be executed
   */
  static void automatedMode(Socket *socket, std::string &address, int port, std::string &filePath);
};