#include "inc/commands/command.hpp"
#include "inc/commands/parser.hpp"
#include "inc/commands/util.hpp"
#include <cassert>

#include <algorithm>

using namespace std;

#define COMMENT_MARKER '#'

bool CommandParser::addBytes(const string &snippet) {
  inputBuf = inputBuf + snippet;

  /* If I have already seen that there is a 
   * newline, I do not need to check that again */
  if (newlinePos != string::npos)
    return true;

  /* Otherwise, check for newline which is where
   * a command is supposed to end */
  auto pos = inputBuf.find_first_of('\n', 0);

  if (pos == string::npos)
    return false;
  else {
    newlinePos = pos;
    return true;
  }
}


bool CommandParser::hasCommand() {
  return newlinePos != string::npos;
}

Command *CommandParser::getCommand() {
  // cout << "Current input (" << inputBuf << ")\n";
  assert(newlinePos != string::npos);

  string firstCommand = inputBuf.substr(0, newlinePos);
  /* BUG POINT: We can ignore the out of bounds problem */
  inputBuf = inputBuf.substr(newlinePos + 1, string::npos);
  newlinePos = inputBuf.find_first_of('\n', 0);

  // cout << "Chosen command (" << firstCommand << ")\n";
  // cout << "Remaining input (" << inputBuf << ")\n";

  vector<string> tokens;
  stringTokenizer(firstCommand, tokens);
  /* We will allow an empty command line with no tokens.
   * I use an invalid command token to ensure that the resulting command is the errCommand */
  if (tokens.empty())
    tokens.emplace_back("error_cmd");
  auto *cmd = constructSpecCommand(tokens[0]);

  for (unsigned int i = 1; i < tokens.size(); i++)
    cmd->addArg(tokens[i]);

  return cmd;
}

CfgFileParser::CfgFileParser(const string &cfgFileName)
    : cfgFile(cfgFileName, ios_base::in) {
  assert(cfgFile.is_open());

  string line;
  /* Checking cfgfile validity */
  while (getline(cfgFile, line)) {
    /* Ignore lines starting with # */
    if (line[line.find_first_not_of(' ', 0)] == COMMENT_MARKER)
      continue;

    /* Break string into tokens */
    vector<string> tokens;
    stringTokenizer(line, tokens);

    /* Skip empty lines */
    if (tokens.empty())
      continue;

    if (tokens[0] == "base")
      assert(tokens.size() == 2);
    else if (tokens[0] == "port")
      /* The user does not need to read server port */
      assert(tokens.size() == 2);
    else if (tokens[0] == "user")
      /* Skip for now, we eill read for valid user/pass later */
      assert(tokens.size() == 3);
    else
      assert(0);
  }
}

CfgFileParser::~CfgFileParser() {
  cfgFile.close();
}

string CfgFileParser::getBaseDir() {
  /* Reset offset in file */
  cfgFile.clear();
  cfgFile.seekg(0, std::fstream::beg);

  string line;
  while (getline(cfgFile, line)) {
    /* Ignore lines starting with # */
    if (line[line.find_first_not_of(' ', 0)] == COMMENT_MARKER)
      continue;

    /* Break string into tokens */
    vector<string> tokens;
    stringTokenizer(line, tokens);

    /* Skip empty lines */
    if (tokens.empty())
      continue;

    if (tokens[0] == "base")
      return tokens[1];
  }

  /* Control should never reach here */
  assert(0);
  return "";
}

int CfgFileParser::getPort() {
  /* Reset offset in file */
  cfgFile.clear();
  cfgFile.seekg(0, std::fstream::beg);

  string line;
  while (getline(cfgFile, line)) {
    /* Ignore lines starting with # */
    if (line[line.find_first_not_of(' ', 0)] == COMMENT_MARKER)
      continue;

    /* Break string into tokens */
    vector<string> tokens;
    stringTokenizer(line, tokens);

    /* Skip empty lines */
    if (tokens.empty())
      continue;

    if (tokens[0] == "port")
      return std::stoi(tokens[1]);
  }

  /* Control should never reach here */
  assert(0);
  return 0;
}

bool CfgFileParser::checkUser(const string &username) {
  /* Reset offset in file */
  cfgFile.clear();
  cfgFile.seekg(0, std::fstream::beg);

  string line;
  while (getline(cfgFile, line)) {
    /* Ignore lines starting with # */
    if (line[line.find_first_not_of(' ', 0)] == COMMENT_MARKER)
      continue;

    /* Break string into tokens */
    vector<string> tokens;
    stringTokenizer(line, tokens);

    /* Skip empty lines */
    if (tokens.empty())
      continue;

    if (tokens[0] == "user")
      if (tokens[1] == username)
        return true;
  }

  return false;
}

bool CfgFileParser::checkUserPass(const string &username, const string &password) {
  /* Reset offset in file */
  cfgFile.clear();
  cfgFile.seekg(0, std::fstream::beg);

  string line;
  while (getline(cfgFile, line)) {
    /* Ignore lines starting with # */
    if (line[line.find_first_not_of(' ', 0)] == COMMENT_MARKER)
      continue;

    /* Break string into tokens */
    vector<string> tokens;
    stringTokenizer(line, tokens);

    /* Skip empty lines */
    if (tokens.empty())
      continue;

    if (tokens[0] == "user")
      if (tokens[1] == username)
        return (tokens[2] == password);
  }

  return false;
}