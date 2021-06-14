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

#include "item.h"

#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

const std::string HACKERNEWS_API_URL = "https://hacker-news.firebaseio.com/v0/";

class Hackernews
{
public:
    Hackernews();

    hnItem getItem(int itemID);

private:

    nlohmann::json get(const std::string &ApiEndpoint);
};

#endif