#include <cassert>
#include <algorithm>

#include <dirent.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "inc/services/threadpool.hpp"
#include "inc/server/server.hpp"
#include "inc/commands/command.hpp"

#define ERROR_STR       "Error: "
#define ACCESS_DENIED   "access denied!"
#define NOT_LOGGED_IN   "not logged in!"
#define PATH_TOO_LONG   "the path is too long."
#define TX_FAIL         "file transfer failed"
#define SUCCESS         "Success: "
#define MAX_FILENAME_SZ 128

int loginCommand::exec(std::string &result, UserState &state) {
  if (state.loggedIn()) {
    result.append(ERROR_STR "Already logged in" "\n");
    return 0;
  }
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }

  if (state.loggedIn()) {
    result.append(ERROR_STR "You are already logged in" "\n");
    return 0;
  }

  if (state.setUser(args[0])) {
    result.append(SUCCESS "\n");
    return 1;
  } else {
    result.append(ERROR_STR "Invalid username\n");
    return 0;
  }
}

int passCommand::exec(std::string &result, UserState &state) {
  string tmp;
  state.getUser(tmp);

  if (state.loggedIn() || !tmp.empty()) {
    result.append(ERROR_STR "Already logged in " "or no user " "\n");
    return 0;
  }
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }

  if (!state.isUsernameSet()) {
    result.append(ERROR_STR "Username is not given" "\n");
    return 0;
  }

  if (state.loggedIn()) {
    result.append(ERROR_STR "You are already logged in" "\n");
    return 0;
  }

  if (state.checkPassLogin(args[0])) {
    result.append(SUCCESS "\n");
    return 1;
  } else {
    result.append(ERROR_STR "Invalid username" " or wrong password\n");
    return 0;
  }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"

int pingCommand::exec(std::string &result, UserState &state) {
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }

  char tmpName[16];
  strncpy(tmpName, "/tmp/pingXXXXXX", 16);
  int tmpFileFD = mkstemp(tmpName);
  if (tmpFileFD == -1) {
    result.append(ERROR_STR "Unable to get temporary file\n");
    return 0;
  }

  char cmd[PATH_MAX];
  snprintf(cmd, PATH_MAX, "ping %s -c 1 >> %15s 2>&1", args[0].c_str(), tmpName);
  /* Ping success value is zero */
  int systemRetCode = system(cmd);
  if (systemRetCode >= 0) {
    char buf[PATH_MAX];
    memset(buf, 0, PATH_MAX);

    while (read(tmpFileFD, buf, PATH_MAX - 1) != 0)
      result.append(buf);
  } else
    result.append(ERROR_STR "ping failed\n");

  close(tmpFileFD);

  return (systemRetCode >= 0);
}

int lsCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (!args.empty()) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }


  char tmpName[16];
  strncpy(tmpName, "/tmp/lsXXXXXX", 16);
  int tmpFileFD = mkstemp(tmpName);
  if (tmpFileFD == -1) {
    result.append(ERROR_STR "Unable to get temporary file" "\n");
    return 0;
  }

  char cmd[PATH_MAX];
  snprintf(cmd, PATH_MAX, "ls -l %s >> %15s 2>&1", state.curDir.c_str(), tmpName);
  /* ls return vals are
   * 0 = OK
   * 1 = minor problems (e.g., cannot access subdirectory)
   * 2 =  if serious trouble (e.g., cannot access command-line arg) */
  int systemRetCode = system(cmd);
  if (systemRetCode == 0) {
    char buf[PATH_MAX];
    memset(buf, 0, PATH_MAX);

    int bRead;
    while ((bRead = read(tmpFileFD, buf, PATH_MAX - 1)) != 0) {
      /* BUG POINT: Make this +1 */
      buf[bRead] = 0;
      result.append(buf);
      memset(buf, 0, PATH_MAX);
    }
  } else
    result.append(ERROR_STR "ls failed" "\n");

  close(tmpFileFD);

  return (systemRetCode == 0);
}

int cdCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  if (args[0].size() > MAX_FILENAME_SZ) {
    result.append(ERROR_STR PATH_TOO_LONG "\n");
    return 0;
  }
  if (args[0][0] == '/') {
    result.append(ERROR_STR ACCESS_DENIED "\n");
    return 0;
  }

  char buf[PATH_MAX];
  memset(buf, 0, PATH_MAX);
  string destination = state.curDir + "/" + args[0];
  char *retPath = realpath(destination.c_str(), buf);
  assert(retPath == buf || (retPath == nullptr));
  /* Check for sandbox breach */
  /* base directory should be a subpath of destination */
  string path(buf);
  if (!state.verifyNewPath(path)) {
    result.append(ERROR_STR ACCESS_DENIED "\n");
    return 0;
  }

  /* Check that final path is directory */
  struct stat statbuf{};
  if ((stat(buf, &statbuf) == 0) && S_ISDIR(statbuf.st_mode)) {
    result.append(SUCCESS "\n");
    state.curDir.clear();
    state.curDir.append(buf);
    return 1;
  } else {
    result.append(ERROR_STR "cd failed" "\n");
    return 0;
  }
}

int mkdirCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  if (args[0].size() > MAX_FILENAME_SZ) {
    result.append(ERROR_STR PATH_TOO_LONG "\n");
    return 0;
  }

  /* Remove final '/' if any */
  if (args[0][args[0].size() - 1] == '/')
    args[0] = args[0].substr(0, args[0].size() - 1);

  char buf[PATH_MAX];
  memset(buf, 0, PATH_MAX);
  string destination = state.curDir + "/" + args[0];
  char *retPath = realpath(destination.c_str(), buf);
  assert(retPath == buf || (retPath == nullptr));
  /* Check for sandbox breach */
  /* base directory should be a subpath of destination */
  string path(buf);
  if (!state.verifyNewPath(path)) {
    result.append(ERROR_STR ACCESS_DENIED "\n");
    return 0;
  }

  /* mkdir with standard 755 permissions */
  if (mkdir(buf, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == 0) {
    result.append(SUCCESS "mkdir" "\n");
    return 1;
  } else {
    result.append(ERROR_STR "mkdir failed" "\n");
    return 0;
  }
}

int rmCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  if (args[0].size() > MAX_FILENAME_SZ) {
    result.append(ERROR_STR PATH_TOO_LONG "\n");
    return 0;
  }

  char buf[PATH_MAX];
  memset(buf, 0, PATH_MAX);
  string filePath = state.curDir + "/" + args[0];
  char *retPath = realpath(filePath.c_str(), buf);
  assert(retPath == buf || (retPath == nullptr));

  if (retPath == nullptr) {
    result.append(ERROR_STR "rm failed" "\n");
    return 0;
  }

  string fileName(retPath);
  if (!state.verifyNewPath(fileName)) {
    result.append(ERROR_STR ACCESS_DENIED "\n");
    return 0;
  }

  if (remove(fileName.c_str()) == 0) {
    result.append(SUCCESS "rm" "\n");
    return 1;
  } else {
    result.append(ERROR_STR "rm failed" "\n");
    return 0;
  }
}

int getCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  if (args[0].size() > MAX_FILENAME_SZ) {
    result.append(ERROR_STR PATH_TOO_LONG "\n");
    return 0;
  }

  state.getPort = 0;
  state.getSize = 0;

  char buf[PATH_MAX];
  memset(buf, 0, PATH_MAX);
  string filePath = state.curDir + "/" + args[0];
  char *retPath = realpath(filePath.c_str(), buf);
  assert(retPath == buf || (retPath == nullptr));

  if (retPath == nullptr) {
    result.append(ERROR_STR "Requested file does not exist" "\n");
    return 0;
  }

  string getFilename(retPath);
  if (!state.verifyNewPath(getFilename)) {
    result.append(ERROR_STR ACCESS_DENIED "\n");
    return 0;
  }
  volatile int *getPortPtr = &state.getPort;
  volatile int *getSizePtr = &state.getSize;
  ThreadPool::run([getFilename, getPortPtr, getSizePtr] {
    Server::getFile(getFilename, getPortPtr, getSizePtr);
  });

  /* Wait until getfile writes the port number and file size */
  while (state.getPort == 0);
  while (state.getSize == 0);
  result.append("get port: ");
  result.append(to_string(state.getPort));
  result.append(" size: ");
  result.append(to_string(state.getSize));
  result.append("\n");

  return 1;
}

int putCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (args.size() != 2) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  if (args[0].size() > MAX_FILENAME_SZ) {
    result.append(ERROR_STR PATH_TOO_LONG "\n");
    return 0;
  }

  state.putPort = 0;

  /* Launch file transfer thread */
  char buf[PATH_MAX];
  memset(buf, 0, PATH_MAX);
  string filePath = state.curDir + "/" + args[0];
  char *retPath = realpath(filePath.c_str(), buf);
  assert(retPath == buf || (retPath == nullptr));
  // if a file does not exist it's ok
  string putFilename;
  if (retPath == nullptr)
    putFilename = filePath;
  else {
    if (!state.verifyNewPath(filePath)) {
      result.append(ERROR_STR ACCESS_DENIED "\n");
      return 0;
    }
    putFilename.append(retPath);
  }

  int putFilesize = atoi(args[1].c_str());
  volatile int *putPortPtr = &state.putPort;
  ThreadPool::run([putFilename, putFilesize, putPortPtr] {
    Server::putFile(putFilename, putFilesize, putPortPtr);
  });

  /* Wait until putfile writes the port number and file size */
  while (state.putPort == 0);
  result.append("put port: ");
  result.append(to_string(state.putPort));
  result.append("\n");

  return 1;
}

int grepCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (args.size() != 1) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  char tmpName[16];
  strncpy(tmpName, "/tmp/grepXXXXXX", 16);
  int tmpFileFD = mkstemp(tmpName);
  if (tmpFileFD == -1) {
    result.append(ERROR_STR "Unable to get temporary file" "\n");
    return 0;
  }

  char cmd[PATH_MAX];
  snprintf(cmd, PATH_MAX, "cd %s; grep -r -l '%s' >> %15s 2>&1", state.curDir.c_str(), args[0].c_str(), tmpName);
  int systemRetCode = system(cmd);
  /* Grep return vals are
  * 0 = line selected
  * 1 = no line selected 
  * 2 = other error          */
  if (systemRetCode != 2) {
    char buf[PATH_MAX];
    memset(buf, 0, sizeof(buf));

    while (read(tmpFileFD, buf, PATH_MAX - 1) != 0) {
      result.append(buf);
      memset(buf, 0, sizeof(buf));
    }
  } else
    result.append(ERROR_STR "grep failed");

  close(tmpFileFD);

  return (systemRetCode == 2);
}

int dateCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (!args.empty()) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  char tmpName[16];
  strncpy(tmpName, "/tmp/dateXXXXXX", 16);
  int tmpFileFD = mkstemp(tmpName);
  if (tmpFileFD == -1) {
    result.append(ERROR_STR "Unable to get temporary file");
    return 0;
  }

  char cmd[PATH_MAX];
  snprintf(cmd, PATH_MAX, "date >> %15s 2>&1", tmpName);
  /* I actually do not know the return value of date */
  int systemRetCode = system(cmd);
  if (systemRetCode == 0) {
    char buf[PATH_MAX];
    memset(buf, 0, sizeof(buf));

    while (read(tmpFileFD, buf, PATH_MAX - 1) != 0)
      result.append(buf);
  } else
    result.append(ERROR_STR "date failed" "\n");

  close(tmpFileFD);

  return (systemRetCode == 0);
}

int whoAmICommand::exec(std::string &result, UserState &state) {
  if (!args.empty()) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }

  state.getUser(result);

  if (result.empty()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  result.append("\n");
  return 1;
}

int wCommand::exec(std::string &result, UserState &state) {
  if (!args.empty()) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }

  state.getUsers(result);
  result.append("\n");
  return 1;
}

int logoutCommand::exec(std::string &result, UserState &state) {
  if (!state.loggedIn()) {
    result.append(ERROR_STR NOT_LOGGED_IN "\n");
    return 0;
  }
  if (!args.empty()) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }

  if (state.logout()) {
    result.append(SUCCESS "logged-out\n");
    return 1;
  } else {
    result.append(ERROR_STR "not logged-in\n");
    return 0;
  }
}

int exitCommand::exec(std::string &result, UserState &state) {
  if (!args.empty()) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }
  /* Logout. If not-logged in, ignore error */
  state.logout();

  state.exit = true;
  return 1;
}

int faultCommand::exec(std::string &result, UserState &state) {
  if (!args.empty()) {
    result.append(ERROR_STR "Wrong number of arguments" "\n");
    return 0;
  }

  /* This is a deliberate null pointer dereference */
  *(int *)NULL = 0;

  return 1;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"

int errCommand::exec(std::string &result, UserState &state) {
  result.append(ERROR_STR "Invalid command \n");

  return 1;
}


Command *constructSpecCommand(const string &cmdStr) {

  if (cmdStr == "login")
    return new loginCommand();
  else if (cmdStr == "pass")
    return new passCommand();
  else if (cmdStr == "ping")
    return new pingCommand();
  else if (cmdStr == "ls")
    return new lsCommand();
  else if (cmdStr == "cd")
    return new cdCommand();
  else if (cmdStr == "mkdir")
    return new mkdirCommand();
  else if (cmdStr == "rm")
    return new rmCommand();
  else if (cmdStr == "get")
    return new getCommand();
  else if (cmdStr == "put")
    return new putCommand();
  else if (cmdStr == "grep")
    return new grepCommand();
  else if (cmdStr == "date")
    return new dateCommand();
  else if (cmdStr == "whoami")
    return new whoAmICommand();
  else if (cmdStr == "w")
    return new wCommand();
  else if (cmdStr == "logout")
    return new logoutCommand();
  else if (cmdStr == "exit")
    return new exitCommand();
  else if (cmdStr == "fault")
    return new faultCommand();
  else
    return new errCommand();
}