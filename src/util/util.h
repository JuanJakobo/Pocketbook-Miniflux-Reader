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
    static void connectToNetwork();

    /**
    * Reads an string from the config file 
    *
    * @param name name of the config that shall be read
    *  
    * @return string that has been found in the config file 
    */
    static std::string readFromConfig(const std::string &name);

    static std::string getData(const std::string &url);

    /**
    * Opens a webpage in a browser 
    * 
    * @param url webpage address that shall be opened
    */
    static void openInBrowser(const std::string &url);

private:
    Util() {}
};
#endif