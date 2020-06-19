#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include "command.hpp"

using namespace std;

class CommandParser {
  string inputBuf;
  size_t newlinePos;

public:
  CommandParser() {
    newlinePos = string::npos;
  }

  /**
   * @brief Add bytes from a stream (such as a socket) to parse into commands
   * @param snippet The snippet read from the stream
   * @return Return whether a full new command has been read
   */
  bool addBytes(const string &snippet);

  /**
   * @brief Checking if a command is available
   * @return Return whether a full new command is available
   */
  bool hasCommand();

  /**
   * @brief Get a command read from the stream. 
   * @return The oldest read command
   * 
   * Returns the oldest command read from the stream and 
   * not previously returned
   */
  Command *getCommand();
};

class CfgFileParser {
  fstream cfgFile;

public:
  explicit CfgFileParser(const string &cfgFileName);

  ~CfgFileParser();

  /**
   * @brief returns the base directory of a the server as specified in the config file
   * @return base dir of the server
   */
  string getBaseDir();

  /**
   * @brief returns the server port specified in the config file
   * @return
   */
  int getPort();

  /**
   * @brief checks if a user with a specific username exists
   * @param username of the client trying to login
   * @return
   */
  bool checkUser(const string &username);

  /**
   * @brief checks if a username/password combination is valid
   * @param username of a client
   * @param password to check
   * @return successful check, false- reject, true- accept
   */
  bool checkUserPass(const string &username, const string &password);
};