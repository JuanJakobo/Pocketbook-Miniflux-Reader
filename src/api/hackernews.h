//------------------------------------------------------------------
// miniflux.h
//
// Author:           JuanJakobo
// Date:             31.05.2021
// Description:
//
//-------------------------------------------------------------------

#ifndef HACKERNEWS
#define HACKERNEWS

#include "hackernewsModel.h"

#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

const std::string HACKERNEWS_API_URL = "https://hacker-news.firebaseio.com/v0/";

class Hackernews
{
public:
    /**
    * Gets the information about an hn item 
    *
    * @param itemID id of the item that shall be downloaded
    *  
    * @return HnItem containing the information of the itemID
    */
    static HnEntry getEntry(int itemID);

    /**
    * Gets the information about a user 
    *
    * @param username 
    *  
    * @return HnUser containing informations
    */
    static HnUser getUser(const std::string &username);

private:
    Hackernews();

    /**
    * Does a get request to the given api endpoint 
    *
    * @param ApiEndpoint information address
    *  
    * @return returns a json object receivt from the api
    */
    static nlohmann::json get(const std::string &apiEndpoint);
};

#endif