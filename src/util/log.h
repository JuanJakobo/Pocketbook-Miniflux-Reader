//------------------------------------------------------------------
// log.h
//
// Author:           JuanJakobo
// Date:             05.08.2020
// Description:      Deals with log entries
//-------------------------------------------------------------------

#ifndef LOG
#define LOG

#include "inkview.h"

#include <string>

class Log
{
  public:

    Log(const std::string &class_name) : _className(class_name)
    {}

    /**
     * Writes a error log entry to the log file
     *
     * @param text that shall be written to the log
     */
    void writeErrorLog(const std::string &text);

    /**
     * Writes a info log entry to the log file
     *
     * @param text that shall be written to the log
     */
    void writeInfoLog(const std::string &text);

  private:
    const std::string _className;

    /**
     * Writes a log entry to the log file
     *
     * @param text that shall be written to the log
     */
    void writeLog(const std::string &text);
};
#endif
