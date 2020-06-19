#pragma once

#include <functional>
#include "inc/services/socket.hpp"
#include "inc/commands/parser.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <string>
#include <set>
#include <map>

class Server {

  // instance fields
  // configuration part
  CfgFileParser *configParser;
  // network part
  Socket *socket;
  // worker thread
  std::thread *workerThread;
  volatile bool exit = false;
  // class instance declarations
  static std::mutex serverCreationMutex;
  static Server *server;
  static bool signalExit;

  /**
   * @brief main server loop to accept connections
   */
  static void loop();

  /**
   * @brief function representing each user connection
   * @param socket through which a user will send commands, function takes ownership of the pointed instance
   * @param userAddressObject of the connected user, function takes ownership of the pointed instance
   */
  static void connectedUser(Socket *socket, SocketAddressIn *userAddressObject);

protected:
  explicit Server(Socket *socket, CfgFileParser *pParser);

public:

  Server(const Server *server) = delete;

  Server(Server &&server) = delete;

  /**
   * @brief startes a server
   */
  void startServer();

  /**
   * @brief wait until main server loop exits
   */
  void wait();

  /**
   * @brief finish server looping and stop all connections and transfers
   */
  void finish();

  ~Server();

  /**
   * @brief make a running server
   * @param socket on which the server will accept connections
   * @param pParser to be used for config files
   * @return pointer to a server instance
   */
  static Server *makeServer(Socket *socket, CfgFileParser *pParser);

  /**
   * @brief get the current server instance
   * @return pointer to a server instance
   */
  static Server *getServer();

  /**
   * @brief function for executing get transfer, should be run on a different thread
   * @param userState representing the user asking for this service
   * @param filename to be sent to the user
   * @param getHandle pointer where
   */
  static void getFile(const string filename, volatile int *getHandle, volatile int *getSize);

  /**
   * @brief function for executing put transfer, should be run on a different thread
   * @param userState
   * @param filename
   * @param fileSize
   * @param putHandle
   */
  static void putFile(const string filename, int fileSize, volatile int *putHandle);
};
