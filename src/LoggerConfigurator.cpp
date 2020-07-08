
// LoggerConfigurator - reading strings for configuring the Logger
//
// Author  : Klaus-Dieter Gundermann
// Created : 30.06.2020
//
// For all support, instructions and copyright go to:
// http://e2guardian.org/
// Released under the GPL v2, with the OpenSSL exception described in the README file.

#ifdef HAVE_CONFIG_H
#include "e2config.h"
#endif

#include <algorithm>
#include <string>
#include "String.hpp"
#include "LoggerConfigurator.hpp"

// -------------------------------------------------------------
// --- Constructor
// -------------------------------------------------------------
LoggerConfigurator::LoggerConfigurator(Logger* logger){
  _logger = logger;
};
LoggerConfigurator::~LoggerConfigurator(){
  _logger = NULL;
};

const std::string LoggerConfigurator::prefix = std::string("log_");
 
// -------------------------------------------------------------
// --- Public Functions
// -------------------------------------------------------------

void LoggerConfigurator::configure(std::string option){

  // option.erase(std::remove(option.begin(), option.end(), " "), option.end());
  String line(option);
  line.removeChar(' ');
  //std::string line = replaceinString(option, " ", "");
  if (!line.startsWith(prefix)) return;

//  size_t pos1 = line.find("=",0);
//  if (pos1 == std::string::npos ) return;
  if (!line.contains("=")) return;


  std::string source = line.after((prefix.c_str())).before("=");

 // std::string source = line.substr(PREFIX.size(), pos1 - PREFIX.size(

  String  destination;
  String filename;

  line = line.after("=");


  if (line.contains(",")) {
    destination = line.before(",");
    filename = line.after(",");
  } else {
    destination = line;
  }


  //isize_t pos2 = line.find(",", pos1);
  //if (pos2 ==  std::string::npos) {
    //destination = line.substr(pos1+1);
    //filename = "";
  //} else
  //{
    //destination = line.substr(pos1+1, pos2-pos1);
    //filename = line.substr(pos2+1);
  //}
  
  LoggerSource src = _logger->string2source(source);
  LoggerDestination dst = _logger->string2dest(destination);
  if (!filename.startsWith("/"))
    filename = std::string(__LOGLOCATION) + filename;
  
  logger_info("LoggerConfig:", " source:", source, " destination:", destination, " filename:", filename );
  _logger->setLogOutput(src, dst, filename);

};

 