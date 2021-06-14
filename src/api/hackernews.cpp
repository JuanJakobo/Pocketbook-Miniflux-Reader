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

#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using std::string;

Hackernews::Hackernews()
{
}

hnItem Hackernews::getItem(int itemID)
{
    nlohmann::json j = get("item/" + std::to_string(itemID) + ".json");
    //TODO check if exists?

    //TODO  if dead --> then is flagged
    hnItem a = hnItem();
    if (j["by"].is_string())
        a.by = j["by"];
    if (j["id"].is_number())
        a.id = j["id"];
    if (j["deleted"].is_boolean())
        a.deleted = j["deleted"];
    //TODO here return error, as is deleted! --> has a deleted comment childs?
    if (j["descendants"].is_number())
        a.descendants = j["descendants"];
    if (j["parent"].is_number())
        a.parent = j["parent"];
    if (j["score"].is_number())
        a.score = j["score"];
    if (j["kids"].is_array())
        a.kids = j["kids"].get<std::vector<int>>();
    if (j["text"].is_string())
        a.text = j["text"];
    if (j["title"].is_string())
        a.title = j["title"];

    return a;
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
                return nlohmann::json::parse(readBuffer);
            default:
                throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
            }
        }
        else
        {
            throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
        }
    }
    return {};
}
