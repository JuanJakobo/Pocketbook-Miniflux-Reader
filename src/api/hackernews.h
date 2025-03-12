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
#include "log.h"
#include "json/json.h"

#include <curl/curl.h>
#include <string>

class Hackernews
{
public:
  Hackernews() = default;

  ~Hackernews();
  /**
     * Gets the information about an hn item
     *
     * @param itemID id of the item that shall be downloaded
     *
     * @return HnItem containing the information of the itemID
     */
  HnEntry getEntry(int itemID);

  /**
     * Gets the information about a user
     *
     * @param username
     *
     * @return HnUser containing informations
     */
  HnUser getUser(const std::string &username);

private:
  Log _logger = Log("hackernews");
  CURL *_curl{nullptr};

  /**
     * Does a get request to the given api endpoint
     *
     * @param ApiEndpoint information address
     *
     * @return returns a json object receivt from the api
     */
  Json::Value get(const std::string &apiEndpoint);
};

#endif
