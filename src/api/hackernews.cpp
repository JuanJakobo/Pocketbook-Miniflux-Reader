//------------------------------------------------------------------
// miniflux.cpp
//
// Author:           JuanJakobo
// Date:             31.05.2021
//
//-------------------------------------------------------------------

#include "hackernews.h"
#include "hackernewsModel.h"
#include "util.h"
#include "log.h"

#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using std::string;

HnEntry Hackernews::getEntry(int entryID)
{
    nlohmann::json j = get("item/" + std::to_string(entryID) + ".json");

    if (j["dead"].is_boolean() || j["deleted"].is_boolean())
    {
        //TODO 
        //Log::writeLogInfo("Item is either dead or flagged. (" + id + ")");
        return {};
    }

    HnEntry entry;

    if (j["by"].is_string())
        entry.by = j["by"];
    
    if (j["id"].is_number())
        entry.id = j["id"];
    
    if (j["time"].is_number())
        entry.time = j["time"];
    
    if (j["descendants"].is_number())
        entry.descendants = j["descendants"];
    
    if (j["parent"].is_number())
        entry.parent = j["parent"];
    
    if (j["score"].is_number())
        entry.score = j["score"];
    
    if (j["kids"].is_array())
        entry.kids = j["kids"].get<std::vector<int>>();
    
    if (j["text"].is_string())
        entry.text = j["text"];

    if (j["title"].is_string())
        entry.title = j["title"];

    return entry;
}

HnUser Hackernews::getUser(const string &username)
{
    nlohmann::json j = get("user/" + username + ".json");

    HnUser user = HnUser();

    if (j["id"].is_string())
        user.id = j["id"];
    if (j["about"].is_string())
        user.about = j["about"];
    if (j["created"].is_number())
        user.created = j["created"];
    if (j["karma"].is_number())
        user.karma = j["karma"];
    if (j["submitted"].is_array())
        user.submitted = j["submitted"].get<std::vector<int>>();

    return user;
}

nlohmann::json Hackernews::get(const string &apiEndpoint)
{

    string url = HACKERNEWS_API_URL + apiEndpoint;

    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {

        struct curl_slist *headers = NULL;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            switch (response_code)
            {
            case 200:
            {
                return nlohmann::json::parse(readBuffer);
            }
            default:
            {
                throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
            }
            }
        }
        else
        {
            throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
        }
    }
    return {};
}
