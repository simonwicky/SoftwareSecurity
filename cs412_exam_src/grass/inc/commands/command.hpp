#include <utility>

#pragma once

#include <string>
#include <vector>

#include <inc/user/user.hpp>

using namespace std;

/* Generic superclass for all Commands */
class Command {
public:
  vector<string> args;

  Command() = default;

  /**
   * @brief adds an argument to the command
   * @param nextArg to be added
   */
  void addArg(const string &nextArg) {
    args.push_back(nextArg);
  }

  /**
   * @brief each specific command has to implement this method with command specific code
   * @param result which will hold the result executing the command
   * @param state representing the user executing this command
   * @return success code, 0- error, 1- success
   */
  virtual int exec(string &result, UserState &state) = 0;
};

class loginCommand : public Command {
public:
  loginCommand() = default;

  int exec(string &result, UserState &state) override;
};

class passCommand : public Command {
public:
  passCommand() = default;

  int exec(string &result, UserState &state) override;
};

class pingCommand : public Command {
public:
  pingCommand() = default;

  int exec(string &result, UserState &state) override;
};

class lsCommand : public Command {
public:
  lsCommand() = default;

  int exec(string &result, UserState &state) override;
};

class cdCommand : public Command {
public:
  cdCommand() = default;

  int exec(string &result, UserState &state) override;
};

class mkdirCommand : public Command {
public:
  mkdirCommand() = default;

  int exec(string &result, UserState &state) override;
};

class rmCommand : public Command {
public:
  rmCommand() = default;

  int exec(string &result, UserState &state) override;
};

class getCommand : public Command {
public:
  getCommand() = default;

  int exec(string &result, UserState &state) override;
};

class putCommand : public Command {
public:
  putCommand() = default;

  int exec(string &result, UserState &state) override;
};

class grepCommand : public Command {
public:
  grepCommand() = default;

  int exec(string &result, UserState &state) override;
};

class dateCommand : public Command {
public:
  dateCommand() = default;

  int exec(string &result, UserState &state) override;
};

class whoAmICommand : public Command {
public:
  whoAmICommand() = default;

  int exec(string &result, UserState &state) override;
};

class wCommand : public Command {
public:
  wCommand() = default;

  int exec(string &result, UserState &state) override;
};

class logoutCommand : public Command {
public:
  logoutCommand() = default;

  int exec(string &result, UserState &state) override;
};

class exitCommand : public Command {
public:
  exitCommand() = default;

  int exec(string &result, UserState &state) override;
};

class faultCommand : public Command {
public:
  faultCommand() = default;

  int exec(string &result, UserState &state) override;
};

class errCommand : public Command {
public:
  errCommand() = default;

  int exec(string &result, UserState &state) override;
};

/**
 * @brief Construct specific subclass object for command
 * @param cmdStr String command
 * @return Pointer to command object
 */
Command *constructSpecCommand(const string &cmdStr);