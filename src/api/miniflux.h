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

#include "minifluxModel.h"

#include <string>
#include <vector>
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

    MfEntry getEntry(int entryID);

    std::vector<MfEntry> getEntries(const std::string &filter);

    MfFeedIcon getFeedIcon(int feedID);

    /**
    * Refreshes all feeds in the background
    *
    *  
    * @return true if feeds upgrade was initiated
    */
    bool refreshAllFeeds();

    bool markUserEntriesAsRead(int userID);


    bool toggleBookmark(int entryID);

    /**
    * Upgrades the status (read, unread) of the selected items 
    *
    * @param entries entries that shall be updated
    * @param read true if items should be marked as read, false if unread
    *  
    * @return true if feeds upgrade was initiated
    */
    bool updateEntries(const std::vector<int> &entries, bool read);

private:
    std::string _url;
    std::string _token;

    bool put(const std::string &apiEndpoint, const std::string &data);

    nlohmann::json get(const std::string &apiEndpoint);
};

#endif