//------------------------------------------------------------------
// miniflux.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "util.h"
#include "log.h"
#include "miniflux.h"
#include "minifluxModel.h"

#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using std::string;
using std::vector;

Miniflux::Miniflux(const string &url, const string &token) : _url(url), _token(token)
{
}

MfEntry Miniflux::getEntry(int entryID)
{
    nlohmann::json element = get("/v1/entries/" + std::to_string(entryID));
    return getEntryLocal(element);
}


vector<MfEntry> Miniflux::getEntries(const string &filter)
{
    nlohmann::json j = get("/v1/entries?" + filter);

    vector<MfEntry> tempItems;

    for (const auto &element : j["entries"].items())
        tempItems.push_back(getEntryLocal(element.value()));

    return tempItems;
}

MfFeedIcon Miniflux::getFeedIcon(int feedID)
{

    nlohmann::json j = get("/v1/feeds" + std::to_string(feedID) + "/icon");

    MfFeedIcon temp;

    if (j["id"].is_number())
        temp.id = j["id"];
    if (j["data"].is_string())
        temp.data = j["data"];
    if (j["mime_type"].is_string())
        temp.mime_type = j["mime_type"];

    //save to storage and update if not exist

    return temp;
}

void Miniflux::refreshAllFeeds()
{
    put("/v1/feeds/refresh", "");
}

void Miniflux::markUserEntriesAsRead(int userID)
{
    put("/v1/users/" + std::to_string(userID) + "/mark-all-as-read", "");
}

void Miniflux::toggleBookmark(int entryID)
{
    put("/v1/entries/" + std::to_string(entryID) + "/bookmark", "");
}

void Miniflux::updateEntries(const vector<int> &entries, bool read)
{
    if (entries.size() <= 0)
        throw std::runtime_error("The size of the entries that shall be updated has to be bigger than 0.");

    string data = "{\"entry_ids\": [";

    for (size_t i = 0; i < entries.size(); i++)
    {
        if (i == 0)
            data.append(std::to_string(entries.at(i)));
        else
            data.append("," + std::to_string(entries.at(i)));
    }

    data.append("], \"status\":");

    if (read)
        data.append("\"read\"");
    else
        data.append("\"unread\"");
    data.append("}");

    put("/v1/entries", data);
}

MfEntry Miniflux::getEntryLocal(const nlohmann::json &element)
{
    MfEntry temp;

    if (element["id"].is_number())
        temp.id = element["id"];
    if (element["status"].is_string())
        temp.status = element["status"];
    if (element["title"].is_string())
        temp.title = element["title"];
    if (element["url"].is_string())
        temp.url = element["url"];
    if (element["comments_url"].is_string())
        temp.comments_url = element["comments_url"];
    if (element["content"].is_string())
        temp.content = element["content"];
    if (element["starred"].is_boolean())
        temp.starred = element["starred"];
    if (element["reading_time"].is_number())
        temp.reading_time = element["reading_time"];

    return temp;
}

void Miniflux::put(const std::string &apiEndpoint, const string &data)
{
    string url = _url + apiEndpoint;

    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("X-Auth-Token: " + _token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
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
                break;
            default:
                Log::writeErrorLog("Miniflux API: " + url + " Curl Response Code: " + std::to_string(response_code));
                throw std::runtime_error("Miniflux API: " + url + " Curl Response Code: " + std::to_string(response_code));
            }
        }
        else
        {
            Log::writeErrorLog("Miniflux API: " + url + " RES Error Code: " + std::to_string(res));
            throw std::runtime_error("Minifllux API: " + url + " RES Error Code: " + std::to_string(res));
        }
    }
}

nlohmann::json Miniflux::get(const string &apiEndpoint)
{

    string url = _url + apiEndpoint;

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
                Log::writeErrorLog("Miniflux API: " + url + " Response Code: " + std::to_string(res));
                throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
            }
        }
        else
        {
            Log::writeErrorLog("Miniflux API: " + url + " RES Error Code: " + std::to_string(res));
            throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
        }
    }
    return {};
}
