#pragma once

#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

class CfgFileParser;

class UserState {
  CfgFileParser &cfg;
  string userName;
  bool isLoggedIn;
  int userPort;
  string defDir;
  string userFile;

  void makeUserFile();

  void rmUserFile();

public:
  UserState(CfgFileParser &cfg, int userPort);

  ~UserState();

  string curDir;
  volatile int getPort;
  volatile int putPort;
  volatile int getSize;
  volatile bool exit = false;
  volatile bool transfer = false;

  /**
   * @brief Obvious */
  bool loggedIn() {
    return isLoggedIn;
  }

  /**
   * @brief Sets username for logging in session
   * @param user Username to be set
   * @return Whether the username is valid as per the config file
   */
  bool setUser(string &user);

  /**
   * @brief checks if a username is set
   * @return true- is set, false- not set
   */
   bool isUsernameSet();

  /**
   * @brief Sets status to logged in if password is valid for userName
   * @param pass Password to be checked
   * @return Whether the user/pass combo is valid as per the config file
   */
  bool checkPassLogin(string &pass);

  /**
   * @brief Log out user, voiding necessary vars */
  bool logout();

  /**
   * @brief Gets username if logged in
   * @param name String reference to write name to
   */
  void getUser(string &name);

  /**
   * @brief Gets list of logged in users
   * @param name String reference to write user names to
   */
  void getUsers(string &name);

  /**
   * @brief Verifies new path for directory change, specifically that it is within base dir
   * @param path String containing new absolute path (no './', '../')
   */
  bool verifyNewPath(string &path);
};

bool createUsersDir();

void rmUsersDir();