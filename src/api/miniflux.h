//------------------------------------------------------------------
// miniflux.h
//
// Author:           JuanJakobo
// Date:             22.04.2021
// Description:
//
//-------------------------------------------------------------------

#ifndef MINIFLUX
#define MINIFLUX

#include "model.h"
#include "util.h"
#include "log.h"

#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

class Miniflux
{
public:
    /**
    * creatas a new Miniflux object containing the url and token to access the api
    *
    * @param url 
    * @param token 
    *  
    */
    Miniflux(const std::string &url, const std::string &token);

    std::vector<entry> getEntries(const std::string &filter);

    feedIcon getFeedIcon(int feedID);

    /**
    * Refreshes all feeds in the background
    *
    *  
    * @return true if feeds upgrade was initiated
    */
    bool refreshAllFeeds();

    bool markUserEntriesAsRead(int userID);

    bool updateEntries(const std::vector<entry> &entrys);

private:
    std::string _url;
    std::string _token;

    bool put(const std::string &apiEndpoint, const std::string &data);

    nlohmann::json get(const std::string &apiEndpoint);
};

#endif