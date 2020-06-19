
#include <string>
#include <inc/services/logging.hpp>
#include <iostream>

void Logging::logDebug(const std::string &message) {
  std::cout << "debug: " << message << std::endl;
}

void Logging::logInfo(const std::string &message) {
  std::cout << "info: " << message << std::endl;
}

void Logging::logWarn(const std::string &message) {
  std::cout << "warn: " << message << std::endl;
}

void Logging::logError(const std::string &message) {
  std::cout << "error: " << message << std::endl;
}

void Logging::logFatal(const std::string &message) {
  std::cout << "fatal " << message << std::endl;
}

void Logging::logDebug(std::string &&message) {
  std::cout << "debug: " << message << std::endl;
}

void Logging::logInfo(std::string &&message) {
  std::cout << "info: " << message << std::endl;
}

void Logging::logWarn(std::string &&message) {
  std::cout << "warn: " << message << std::endl;
}

void Logging::logError(std::string &&message) {
  std::cout << "error: " << message << std::endl;
}

void Logging::logFatal(std::string &&message) {
  std::cout << "fatal " << message << std::endl;
}

void Logging::logDebug(const char *message) {
  printf("debug: ");printf(message);printf("\n");
}

void Logging::logInfo(const char *message) {
  printf("info: ");printf(message);printf("\n");
}

void Logging::logWarn(const char *message) {
  printf("warning: ");printf(message);printf("\n");
}

void Logging::logError(const char *message) {
  printf("error: ");printf(message);printf("\n");
}

void Logging::logFatal(const char *message) {
  printf("fatal: ");printf(message);printf("\n");
}

