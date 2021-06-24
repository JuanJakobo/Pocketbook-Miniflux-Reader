//------------------------------------------------------------------
// miniflux.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "model.h"
#include "util.h"
#include "log.h"
#include "miniflux.h"

#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using std::string;
using std::vector;

Miniflux::Miniflux(const string &url, const string &token) : _url(url), _token(token)
{
}

vector<entry> Miniflux::getEntries(const string &filter)
{
    //default limit is 100, therefore set limit here to more 
    //TODO make filter variable
    nlohmann::json j = get("/v1/entries?" + filter + "&limit=1000");

    vector<entry> tempItems;

    for (const auto &element : j["entries"].items())
    {
        entry temp;

        if (element.value()["id"].is_number())
            temp.id = element.value()["id"];
        if (element.value()["status"].is_string())
            temp.status = element.value()["status"];
        if (element.value()["title"].is_string())
            temp.title = element.value()["title"];
        if (element.value()["url"].is_string())
            temp.url = element.value()["url"];
        if (element.value()["comments_url"].is_string())
            temp.comments_url = element.value()["comments_url"];
        if (element.value()["content"].is_string())
            temp.content = element.value()["content"];
        if (element.value()["starred"].is_boolean())
            temp.starred = element.value()["starred"];
        if (element.value()["reading_time"].is_number())
            temp.reading_time = element.value()["reading_time"];

        tempItems.push_back(temp);
    }
    return tempItems;
}

feedIcon Miniflux::getFeedIcon(int feedID)
{

    nlohmann::json j = get("/v1/feeds" + std::to_string(feedID) + "/icon");

    feedIcon temp;

    if (j["id"].is_number())
        temp.id = j["id"];
    if (j["data"].is_string())
        temp.data = j["data"];
    if (j["mime_type"].is_string())
        temp.mime_type = j["mime_type"];

    //save to storage and update if not exist

    return temp;
}

bool Miniflux::refreshAllFeeds()
{
    return put("/v1/feeds/refresh", "");
}

bool Miniflux::markUserEntriesAsRead(int userID)
{
    return put("/v1/users/" + std::to_string(userID) + "/mark-all-as-read", "");
}

bool Miniflux::updateEntries(const vector<entry> &entrys)
{
    //TODO only can change status read /unread...
    //string data = "{\"entry_ids\": [6655, 6646], \"status\": \"unread\"}"; //\"starred\": \"true\"}";
    
    //return put("/v1/entries/", data);
    return false;
}

bool Miniflux::put(const std::string &apiEndpoint, const string &data)
{
    string url = _url + apiEndpoint;
    Log::writeLog(url);

    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("X-Auth-Token: " + _token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        if (!data.empty())
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            switch (response_code)
            {
            case 204:
                return true;
                break;
            default:
                throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
            }
        }
        else
        {
            throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
        }
    }
    return false;
}

nlohmann::json Miniflux::get(const string &apiEndpoint)
{

    string url = _url + apiEndpoint;
    Log::writeLog(url);

    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("X-Auth-Token: " + _token).c_str());
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
    //TODO write error code here ?
    return {};
}
