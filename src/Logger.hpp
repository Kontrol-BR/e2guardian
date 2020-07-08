// Logger class - for central logging to console/syslog/file
//
// Author  : Klaus-Dieter Gundermann
// Created : 24.06.2020
//
// For all support, instructions and copyright go to:
// http://e2guardian.org/
// Released under the GPL v2, with the OpenSSL exception described in the README file.

#ifndef __HPP_LOGGINpre
#define __HPP_LOGGINpre

#include "LoggerReal.hpp"



extern Logger e2logger;
extern thread_local std::string thread_id;

#endif
