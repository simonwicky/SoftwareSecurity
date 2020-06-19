#include <cassert>
#include "inc/commands/parser.hpp"
#include "inc/user/user.hpp"
#include "inc/commands/util.hpp"
#include "inc/services/threadpool.hpp"

/* For realpath(...) */
#include <dirent.h>
#include <fcntl.h>
#include <climits>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

/* For starting a data transfer*/
#include <functional>

#include <cerrno>
#include <iostream>
#include <cstring>

#define USERS_FILE_DIR  "/tmp/server_users"

UserState::UserState(CfgFileParser &cfg, int userPort)
    : cfg(cfg), userPort(userPort) {
  isLoggedIn = false;
  string line;
  getPort = 0;
  putPort = 0;
  getSize = 0;

  char *realPath = realpath(cfg.getBaseDir().c_str(), nullptr);
  curDir.append(realPath);
  defDir.append(realPath);
  free(realPath);
}

bool UserState::setUser(string &name) {
  if (cfg.checkUser(name)) {
    userName.clear();
    userName.append(name);
    return true;
  }
  return false;
}

UserState::~UserState() {
  /* Cleanup side effects: 
   * 1. remove users file */
  if (isLoggedIn)
    logout();
}

bool UserState::checkPassLogin(string &pass) {
  assert(!userName.empty());
  assert(!isLoggedIn);

  isLoggedIn = cfg.checkUserPass(userName, pass);
  if (isLoggedIn)
    makeUserFile();

  return isLoggedIn;
}

bool UserState::logout() {
  if (!isLoggedIn)
    return false;

  isLoggedIn = false;
  /* Reset current directory to cfg-ed default */
  curDir.clear();
  curDir.append(defDir);

  rmUserFile();
  userName.clear();

  return true;
}

void UserState::getUser(string &name) {
  if (isLoggedIn)
    name.append(userName);
}

void UserState::makeUserFile() {
  char buf[128];
  sprintf(buf, "%s/%sXXXXXX", USERS_FILE_DIR, userName.c_str());
  /* Create users file if it does not exist */
  int userFileFD = mkstemp(buf);
  assert (userFileFD != -1);
  userFile.append(buf);
  close(userFileFD);
}

void UserState::rmUserFile() {
  remove(userFile.c_str());
  userFile.clear();
}

void UserState::getUsers(string &name) {
  DIR *dir = opendir(USERS_FILE_DIR);
  struct dirent *dirEntry;
  assert(dir);

  set<string> names;
  while ((dirEntry = readdir(dir)) != nullptr) {
    string fileName(dirEntry->d_name);
    if (fileName == "." || fileName == "..")
      continue;

    string otherUser = fileName.substr(0, fileName.length() - 6);
    if (names.find(otherUser) == names.end()) {
      name.append(otherUser);
      name.append(" ");
      names.insert(otherUser);
    }
  }
  int retCl = closedir(dir);
  assert(retCl == 0);
}

bool UserState::verifyNewPath(string &path) {
  if (defDir.length() > path.length())
    return false;

  return (path.substr(0, defDir.length()) == defDir);
}

bool UserState::isUsernameSet() {
  return !userName.empty();
}

bool createUsersDir() {
  return (mkdir(USERS_FILE_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0);
}

void rmUsersDir() {
  /* First step: clear out the USER_FILE_DIR folder of any remaining user files */
  DIR *dir = opendir(USERS_FILE_DIR);
  struct dirent *dirEntry;
  assert(dir);

  while ((dirEntry = readdir(dir)) != nullptr) {
    string fileName(USERS_FILE_DIR);
    fileName.append("/");
    fileName.append(dirEntry->d_name);
    /* Delete all regular files */
    struct stat statbuf{};
    int retStat = stat(fileName.c_str(), &statbuf);
    assert(retStat == 0);
    if (S_ISREG(statbuf.st_mode)) {
      int retRm = remove(fileName.c_str());
      assert(retRm == 0);
    }
  }
  int retCl = closedir(dir);
  assert(retCl == 0);

  /* Finally, remove the directory */
  int retRm = rmdir(USERS_FILE_DIR);
  assert(retRm == 0);
}