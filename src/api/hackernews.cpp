//------------------------------------------------------------------
// miniflux.cpp
//
// Author:           JuanJakobo
// Date:             31.05.2021
//
//-------------------------------------------------------------------

#include "hackernews.h"
#include "item.h"
#include "util.h"
#include "log.h"

#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using std::string;

hnItem Hackernews::getItem(int itemID)
{

    nlohmann::json j = get("item/" + std::to_string(itemID) + ".json");
    Log::writeLog("got item " + std::to_string(itemID));
    hnItem item = hnItem();
    if (j["by"].is_string())
        item.by = j["by"];
    if (j["id"].is_number())
        item.id = j["id"];
    if (j["time"].is_number())
        item.time = j["time"];
    if (j["descendants"].is_number())
        item.descendants = j["descendants"];
    if (j["parent"].is_number())
        item.parent = j["parent"];
    if (j["score"].is_number())
        item.score = j["score"];
    if (j["kids"].is_array())
        item.kids = j["kids"].get<std::vector<int>>();
    if (j["dead"].is_boolean() || j["deleted"].is_boolean())
    {
        if (j["dead"].is_boolean())
        {
            item.text = "[flagged]";
        }
        else if (j["deleted"].is_boolean())
        {
            item.text = "[deleted]";
        }
        Log::writeLog("return item " + std::to_string(item.id));

        return item;
    }
    if (j["text"].is_string())
        item.text = j["text"];

    if (j["title"].is_string())
        item.title = j["title"];
    Log::writeLog("return item " + std::to_string(item.id));
    return item;
}

hnUser Hackernews::getUser(const string &username)
{
    nlohmann::json j = get("user/" + username + ".json");

    hnUser user = hnUser();

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

    Log::writeLog("start curl for " + apiEndpoint);

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
                Log::writeLog("end curl for " + apiEndpoint);
                return nlohmann::json::parse(readBuffer);
            }
            default:
            {
                Log::writeLog("curl error " + std::to_string(response_code));
                throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
            }
            }
        }
        else
        {
            Log::writeLog("curl error " + std::to_string(res));
            throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
        }
    }
    return {};
}
