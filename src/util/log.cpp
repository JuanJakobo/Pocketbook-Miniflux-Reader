//------------------------------------------------------------------
// log.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "log.h"
#include "eventHandler.h"

#include <fstream>
#include <string>

void Log::writeInfoLog(const std::string &text)
{
    writeLog("[0]: " + text);
}

void Log::writeErrorLog(const std::string &text)
{
    writeLog("[1]: " + text);
}

void Log::writeLog(const std::string &text)
{
    std::ofstream log(CONFIG_FOLDER + std::string("/logfile.txt"), std::ios_base::app | std::ios_base::out);

    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%b %d %H:%M:%S", timeinfo);

    log << buffer << ' ' << _className << text << "\n";

    log.close();
}
