// Logger class - for central logging to console/syslog/file
//
// Author  : Klaus-Dieter Gundermann
// Created : 24.06.2020
//
// For all support, instructions and copyright go to:
// http://e2guardian.org/
// Released under the GPL v2, with the OpenSSL exception described in the README file.

#ifdef HAVE_CONFIG_H
#include "e2config.h"
#endif
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <syslog.h>
#include "LoggerReal.hpp"

extern thread_local std::string thread_id;

// -------------------------------------------------------------
// --- Constructor
// -------------------------------------------------------------
Logger::Logger() {
  setSyslogName("Logger");

  setLogOutput(LoggerSource::info,  LoggerDestination::stdout);
  setLogOutput(LoggerSource::error, LoggerDestination::stderr);
  setLogOutput(LoggerSource::access, LoggerDestination::file, "access.log");

  setLogOutput(LoggerSource::debug, LoggerDestination::stdout);
  setLogOutput(LoggerSource::trace, LoggerDestination::stdout);
}

Logger::~Logger() {
  closelog();
}
 // moved to LoggerReal.hpp
//const std::string Logger::SOURCES[] = {"info", "error", "access", "config", "story", "icap", "icapc", "clamav", "thhtps", \
//                                      "debug", "trace", "debugnet", "debugsb", "debugchunk", "debugregexp"};
//const std::string Logger::DESTINATIONS[] = {"none", "stdout", "stderr", "syslog", "file"};

// -------------------------------------------------------------
// --- Helper
// -------------------------------------------------------------

struct Logger::Helper
{
  static std::string build_message(
      const std::string prepend,      
      const std::string func, 
      const int line, 
      const std::string what)
  {
    std::stringstream message;
    if (prepend != "") 
      message << "[" << prepend << "] ";
    if (thread_id != "" && thread_id != "log: ") 
      message << "(" << thread_id << ") ";
#ifdef E2DEBUG
    if (func != "") {
      message << " " << func;
      if (line > 0)
        message << ":" << line << " ";
    };
#endif
    message << what;
    return message.str();
  }

  static void sendToLogfile(const std::string filename, const std::string message){
    if (filename=="")
      return;
    std::ofstream logfile;
    logfile.open(filename,std::ofstream::out | std::ofstream::app );
    logfile << message << std::endl;
    logfile.close();
  }
};

// -------------------------------------------------------------
// --- static Functions
// -------------------------------------------------------------

LoggerSource Logger::string2source(std::string source){
  for( int i=0; i < LS_MAX_VALUE; i++)
  {
    if (SOURCES[i] == source) return static_cast<LoggerSource>(i);
  }
  return LoggerSource::info;
}
LoggerDestination Logger::string2dest(std::string destination){
  for( int i=0; i < LD_MAX_VALUE; i++)
  {
    if (DESTINATIONS[i] == destination) return static_cast<LoggerDestination>(i);
  }
  return LoggerDestination::none;
}

std::string Logger::source2string(LoggerSource source){
  return SOURCES[static_cast<int>(source)];
}
std::string Logger::dest2string(LoggerDestination dest){
  return DESTINATIONS[static_cast<int>(dest)];
}


// -------------------------------------------------------------
// --- Properties
// -------------------------------------------------------------

void  Logger::setSyslogName(const std::string logname){
  _logname = logname;
  closelog();
  openlog(logname.c_str(), LOG_PID | LOG_CONS, LOG_USER);
};

void Logger::enable(const LoggerSource source){
  _enabled[static_cast<int>(source)] = true;
};
void Logger::disable(const LoggerSource source){
  _enabled[static_cast<int>(source)] = false;
};
bool Logger::isEnabled(const LoggerSource source){
  return _enabled[static_cast<int>(source)];
};

void Logger::setLogOutput(const LoggerSource source, const LoggerDestination destination, const std::string filename){
  _destination[static_cast<int>(source)] = destination;
  if (filename.front() == '/')
    // absolute path
    _filename[static_cast<int>(source)] = filename;
  else
    // relative to __LOGLOCATION
    _filename[static_cast<int>(source)]  = std::string(__LOGLOCATION) + filename;

  if (destination == LoggerDestination::none)
    disable(source);
  else
    enable(source);  
}  

void Logger::setDockerMode(){
  // docker stdout/stderr are not in sync
  // so for debugging send everything to stderr (unbuffered)
  setLogOutput(LoggerSource::info, LoggerDestination::stderr);
  setLogOutput(LoggerSource::error, LoggerDestination::stderr);
  setLogOutput(LoggerSource::debug, LoggerDestination::stderr);
  setLogOutput(LoggerSource::trace, LoggerDestination::stderr);
}


// -------------------------------------------------------------
// --- Public Functions
// -------------------------------------------------------------

void Logger::log(const LoggerSource source, const std::string func, const int line, const std::string message )
{
  std::string tag;
  std::string msg;
  if (source > LoggerSource::access) {
    tag=source2string(source);
    msg=Helper::build_message(tag, func, line, message);
  } else {
    // no tags,func,line for: info,error,access
    msg=Helper::build_message("", "", 0, message);
  }
  sendMessage(source, msg);
};


// -------------------------------------------------------------
// --- Private Functions
// -------------------------------------------------------------

void Logger::sendMessage(const LoggerSource source, const std::string message){
  LoggerDestination destination = _destination[static_cast<int>(source)];
  int loglevel;

  switch (destination) {
    case LoggerDestination::none: 
      break;
    case LoggerDestination::stdout:
      std::cout << message << std::endl;
      break;
    case LoggerDestination::stderr:
      std::cerr << message << std::endl;
      break;
    case LoggerDestination::syslog:
      if (source == LoggerSource::error)
        loglevel = LOG_ERR;
      else if (source >= LoggerSource::debug)
        loglevel = LOG_DEBUG;
      else
        loglevel = LOG_INFO;
      syslog(loglevel, "%s", message.c_str());
      break;
    case LoggerDestination::file:
      Helper::sendToLogfile(_filename[static_cast<int>(source)], message);
      break;
  }

}

// -------------------------------------------------------------
// --- Global Logger
// -------------------------------------------------------------

Logger logger;
