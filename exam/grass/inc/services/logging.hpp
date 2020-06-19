#pragma once

#include <string>

#pragma GCC diagnostic ignored "-Wformat-security"

#ifdef DEBUG
  #define INFO
  #define LOG_DEBUG(x) Logging::logDebug(x)
#else
  #define LOG_DEBUG(x)
#endif

#ifdef INFO
  #define  WARN
  #define LOG_INFO(x) Logging::logInfo(x)
#else
  #define LOG_INFO(x)
#endif

#ifndef WARN
  #define ERROR
  #define LOG_WARN(x) Logging::logWarn(x)
#else
  #define LOG_WARN(x)
#endif

#ifdef ERROR
  #define FATAL
  #define LOG_ERROR(x) Logging::logError(x)
#else
  #define LOG_ERROR(x)
#endif

#ifdef FATAL
  #define LOG_FATAL(x) Logging::logFatal(x)
#else
  #define LOG_FATAL(x)
#endif

class Logging {
public:
  // for lvalues
  static void logDebug(const std::string &message);

  static void logInfo(const std::string &message);

  static void logWarn(const std::string &message);

  static void logError(const std::string &message);

  static void logFatal(const std::string &message);

  // for rvalues
  static void logDebug(std::string &&message);

  static void logInfo(std::string &&message);

  static void logWarn(std::string &&message);

  static void logError(std::string &&message);

  static void logFatal(std::string &&message);

  // C style
  static void logDebug(const char *message);

  static void logInfo(const char *message);

  static void logWarn(const char *message);

  static void logError(const char *message);

  static void logFatal(const char *message);
};
