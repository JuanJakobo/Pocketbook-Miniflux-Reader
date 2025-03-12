#pragma once
//------------------------------------------------------------------
// miniflux.h
//
// Author:           JuanJakobo
// Date:             22.04.2021
// Description:
//
//-------------------------------------------------------------------
#include "minifluxModel.h"
#include "log.h"
#include "json/json.h"

#include <string>
#include <vector>

class Miniflux
{
  public:
    /**
     * creats a new Miniflux object containing the url and token to access the api
     *
     * @param url
     * @param token
     *
     */
    Miniflux(const std::string &url, const std::string &token);

    /**
     * returns the information for an entryID
     *
     * @param entryID the id of the item that shall be downloaded
     * @return MfEntry item containing the information
     *
     */
    MfEntry getEntry(int entryID) const;

    /**
     * gets multple entries that fit into the requested filter
     *
     * @param entryID the id of the item that shall be downloaded
     * @return vector of MfEntrys that fit into the filter
     *
     */
    std::vector<MfEntry> getEntries(const std::string &filter) const;

    MfFeedIcon getFeedIcon(int feedID) const;

    /**
     * Refreshes all feeds in the background
     *
     */
    void refreshAllFeeds() const;

    void markUserEntriesAsRead(int userID) const;

    void toggleBookmark(int entryID) const;

    /**
     * Upgrades the status (read, unread) of the selected items
     *
     * @param entries entries that shall be updated
     * @param read true if items should be marked as read, false if unread
     *
     */
    void updateEntries(const std::vector<int> &entries, bool read) const;

  private:
    Log _logger = Log("miniflux");
    std::string _url;
    std::string _token;
    bool _ignoreCert;

    MfEntry getEntryLocal(const Json::Value &element) const;

    void put(const std::string &apiEndpoint, const std::string &data) const;

    Json::Value get(const std::string &apiEndpoint) const;
};
