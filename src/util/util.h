//------------------------------------------------------------------
// util.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      Various utility methods
//-------------------------------------------------------------------

#ifndef UTIL
#define UTIL

#include "inkview.h"

#include <string>

class Util
{
public:
    /**
    * Handles the return of curl command
    * 
    */
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);

    /**
    * Saves the return of curl command
    * 
    */
    static size_t writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);

    /**
    * Checks if a network connection can be established
    * 
    */
    static bool connectToNetwork();

    /**
    * Write an string to an config file 
    *
    * @param name name of the config that shall be written
    * @param value string that shall be written to the config
    *  
    */
    static void writeToConfig(const std::string &name, const std::string &value);
    
    /**
    * Reads an string from the config file 
    *
    * @param name name of the config that shall be read
    *  
    * @return string that has been found in the config file 
    */
    static std::string readFromConfig(const std::string &name);

    static std::string getData(const std::string &url);

    static void decodeHTML(std::string &data);

    static void replaceAll(std::string &data, const std::string &replace, const std::string &by);

private:
    Util() {}
};
#endif
