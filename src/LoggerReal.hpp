// Logger class - for central logging to console/syslog/file
//
// Author  : Klaus-Dieter Gundermann
// Created : 24.06.2020
//
// For all support, instructions and copyright go to:
// http://e2guardian.org/
// Released under the GPL v2, with the OpenSSL exception described in the README file.

#ifndef __HPP_LOGGING
#define __HPP_LOGGING

#include <sstream>

// only C++14 : using namespace std::string_literals;

enum class LoggerSource {
  // used in production:
  info, error, access, config, story, icap, icapc, clamav, thhtps,
  // only usable when compiled with E2DEBUG:
  debug, trace, debugnet, debugsb, debugchunk, debugregexp
// ,  __MAX_VALUE
};
#define LS_MAX_VALUE 15

enum class LoggerDestination {
  none, stdout, stderr, syslog, file
  //,__MAX_VALUE
};
#define LD_MAX_VALUE 5

class Logger
{
  public:
  Logger();   // constructor
  ~Logger();  // destructor

  void setSyslogName(const std::string logname);

  // enable/disable Logging Sources
  void enable(const LoggerSource source);
  void disable(const LoggerSource source);
  bool isEnabled(const LoggerSource source);

  void setLogOutput(const LoggerSource source, const LoggerDestination destination, const std::string filename="");

  void setDockerMode();

  void log(const LoggerSource source, const std::string func, const int line, const std::string message );

  // Conversion Enum <-> string
 // static const std::string SOURCES[static_cast<int>(LoggerSource::__MAX_VALUE)];
 // static const std::string DESTINATIONS[static_cast<int>(LoggerDestination::__MAX_VALUE)];
//  std::string SOURCES[static_cast<int>(LoggerSource::__MAX_VALUE)] =
   const std::string SOURCES[LS_MAX_VALUE] =
          {"info", "error", "access", "config", "story", "icap", "icapc", "clamav", "thhtps",
           "debug", "trace", "debugnet", "debugsb", "debugchunk", "debugregexp"};
  const std::string DESTINATIONS[LD_MAX_VALUE] = {"none", "stdout", "stderr", "syslog", "file"};

  LoggerSource string2source(std::string source);
  LoggerDestination string2dest(std::string destination);

  std::string source2string(LoggerSource source);
  std::string dest2string(LoggerDestination dest);

  // Variable Args
  template<typename T>  void cat_vars(std::stringstream &mess, T e) {
      mess << e;
  }
  template<typename T, typename... Args>  void cat_vars(std::stringstream &mess, T e, Args... args) {
      mess << e;
      cat_vars(mess, args...);
  }
  template<typename... Args> std::string cat_all_vars(Args... args) {
      std::stringstream mess;
      cat_vars(mess, args...);
      return mess.str();
  }
  template<typename... Args>  void vlog(const LoggerSource source, const std::string func, const int line, Args... args) {
      log(source, func, line, cat_all_vars(args...));
  };

  private:
  std::string _logname;

  bool _enabled[LS_MAX_VALUE];
  LoggerDestination _destination[LS_MAX_VALUE];
  std::string _filename[LS_MAX_VALUE];

  struct Helper;

  void sendMessage(const LoggerSource source, const std::string message);
};

#define logger_info(...)  \
  if (e2logger.isEnabled(LoggerSource::info)) \
     e2logger.vlog(LoggerSource::info,  __func__, __LINE__, __VA_ARGS__)
#define logger_error(...) \
  if (e2logger.isEnabled(LoggerSource::error)) \
     e2logger.vlog(LoggerSource::error,  __func__, __LINE__, __VA_ARGS__)
#define logger_access(...)  \
  if (e2logger.isEnabled(LoggerSource::access)) \
     e2logger.vlog(LoggerSource::access,  __func__, __LINE__, __VA_ARGS__)
#define logger_config(...) \
  if (e2logger.isEnabled(LoggerSource::config)) \
     e2logger.vlog(LoggerSource::config,  __func__, __LINE__, __VA_ARGS__)
#define logger_story(...) \
  if (e2logger.isEnabled(LoggerSource::story)) \
    e2logger.vlog(LoggerSource::story, "", 0,  __VA_ARGS__)


#ifdef E2DEBUG
  #define logger_debug(...) \
    if (e2logger.isEnabled(LoggerSource::debug)) \
      e2logger.vlog(LoggerSource::debug,  __func__, __LINE__, __VA_ARGS__)
  #define logger_trace(...) \
    if (e2logger.isEnabled(LoggerSource::trace)) \
      e2logger.vlog(LoggerSource::trace,  __func__, __LINE__, __VA_ARGS__)
  #define logger_debugnet(...) \
    if (e2logger.isEnabled(LoggerSource::debugnet)) \
      e2logger.vlog(LoggerSource::debugnet,  __func__, __LINE__, __VA_ARGS__)
  #define logger_debugregexp(...) \
    if (e2logger.isEnabled(LoggerSource::debugregexp)) \
      e2logger.vlog(LoggerSource::debugregexp,  __func__, __LINE__, __VA_ARGS__)
  #define logger_debugsb(...) \
    if (e2logger.isEnabled(LoggerSource::debugsb)) \
      e2logger.vlog(LoggerSource::debugsb,  __func__, __LINE__, __VA_ARGS__)

#else
  #define logger_debug(...)
  #define logger_trace(...)
  #define logger_debugnet(...)
  #define logger_debugregexp(...)
  #define logger_debugsb(...)
#endif


#endif
