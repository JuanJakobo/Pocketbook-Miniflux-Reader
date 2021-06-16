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
    Miniflux(const std::string &url, const std::string &token);

    std::vector<entry> getEntries(const std::string &filter);

    bool markUserEntriesAsRead(int userID);

    bool updateEntries(const std::vector<entry> &entrys);

private:
    std::string _url;
    std::string _token;

    nlohmann::json get(const std::string &apiEndpoint);
};

#endif