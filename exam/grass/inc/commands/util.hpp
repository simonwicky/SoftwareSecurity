#pragma once

#include <sstream>
#include <string> 
#include <vector>

/**
 * @brief splits the string on whitespaces
 * @param in string to be split
 * @param out vector to hold the splits
 */
inline void stringTokenizer(const std::string& in, std::vector<std::string> &out) {
  std::stringstream lineStream(in);
  std::string token;

  while(getline(lineStream, token, ' '))
    if(!token.empty())
      out.push_back(token);
}