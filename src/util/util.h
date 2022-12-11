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

enum class Action
{
    IWriteSecret,
    IReadSecret,
    IWriteString,
    IReadString
};

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
    * Enables the access to the config file
    *
    * @param actions taht shall be performed
    * @param name of the config that shall be read
    * @param value that shall be written to the config
    *
    * @return string that has been found in the config file
    */
    static std::string accessConfig(const Action &action, const std::string &name, const std::string &value = std::string());

    static std::string getData(const std::string &url);

    static void decodeHTML(std::string &data);

    /**
     * Removes chars that are forbidden in an path
     *
     * @param string that has to be changed
     * @return string that has been adjusted
     */
    static std::string clearString(std::string title);

    static std::string returnFolderName(std::string title);

    /**
     * Creates an html file, downloades the pictures and saves it to path
     *
     * @param title the name the html should be saved
     * @param content the html content
     *
     * @return path where the html file is saved to
     */
    static std::string createHtml(const std::string& title, std::string content);

    static void replaceAll(std::string &data, const std::string &replace, const std::string &by);

private:
    Util() {}
};
#endif
