//------------------------------------------------------------------
// miniflux.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "miniflux.h"
#include "log.h"
#include "minifluxModel.h"
#include "util.h"

#include <curl/curl.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

Miniflux::Miniflux(const string &url, const string &token) : _url(url), _token(token)
{
    _ignoreCert = Util::accessConfig(Action::IReadString, "Cert") == "ignore";
    if (_ignoreCert)
    {
        Log::writeInfoLog("ignoring certs");
    }
}

MfEntry Miniflux::getEntry(int entryID)
{
    const auto element{get("/v1/entries/" + std::to_string(entryID))};
    return getEntryLocal(element);
}

vector<MfEntry> Miniflux::getEntries(const string &filter)
{
    const auto j{get("/v1/entries?" + filter)};

    vector<MfEntry> tempItems;

    for (const auto &element : j["entries"])
    {
        tempItems.push_back(getEntryLocal(element));
    }

    return tempItems;
}

MfFeedIcon Miniflux::getFeedIcon(int feedID)
{

    const auto j{get("/v1/feeds" + std::to_string(feedID) + "/icon")};

    MfFeedIcon temp;

    if (j["id"].isNumeric())
    {
        temp.id = j["id"].asInt();
    }
    if (j["data"].isString())
    {
        temp.data = j["data"].asString();
    }
    if (j["mime_type"].isString())
    {
        temp.mime_type = j["mime_type"].asString();
    }

    // save to storage and update if not exist

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
    if (entries.size() == 0)
    {
        throw std::runtime_error("The size of the entries that shall be updated has to be bigger than 0.");
    }

    std::string data{"{\"entry_ids\": ["};

    for (size_t i = 0; i < entries.size(); i++)
    {
        if (i == 0)
        {
            data.append(std::to_string(entries.at(i)));
        }
        else
        {
            data.append("," + std::to_string(entries.at(i)));
        }
    }

    data.append("], \"status\":");

    if (read)
    {
        data.append("\"read\"");
    }
    else
    {
        data.append("\"unread\"");
    }
    data.append("}");

    put("/v1/entries", data);
}

MfEntry Miniflux::getEntryLocal(const Json::Value &element)
{
    MfEntry temp;

    if (element["id"].isNumeric())
    {
        temp.id = element["id"].asInt();
    }
    if (element["status"].isString())
    {
        temp.status = element["status"].asString();
    }
    if (element["title"].isString())
    {
        temp.title = element["title"].asString();
    }
    if (element["url"].isString())
    {
        temp.url = element["url"].asString();
    }
    if (element["comments_url"].isString())
    {
        temp.comments_url = element["comments_url"].asString();
    }
    if (element["content"].isString())
    {
        temp.content = element["content"].asString();
    }
    if (element["starred"].isBool())
    {
        temp.starred = element["starred"].asBool();
    }
    if (element["reading_time"].isNumeric())
    {
        temp.reading_time = element["reading_time"].asInt();
    }

    return temp;
}

void Miniflux::put(const std::string &apiEndpoint, const string &data)
{

    Util::connectToNetwork();
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

        if (_ignoreCert)
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        if (!data.empty())
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        }
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
                throw std::runtime_error("Miniflux API: " + url +
                                         " Curl Response Code: " + std::to_string(response_code));
            }
        }
        else
        {
            Log::writeErrorLog("Miniflux API: " + url + " RES Error Code: " + std::to_string(res));
            throw std::runtime_error("Minifllux API: " + url + " RES Error Code: " + std::to_string(res));
        }
    }
}

Json::Value Miniflux::get(const string &apiEndpoint)
{

    Util::connectToNetwork();
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

        if (_ignoreCert)
        {
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        }

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            switch (response_code)
            {
            case 200: {
                Json::Value root;
                Json::Reader reader;
                if (reader.parse(readBuffer, root))
                {
                    return root;
                }
                constexpr auto err_msg{"Failed to parse Response to json"};
                Log::writeErrorLog(err_msg);
                throw std::runtime_error(err_msg);
            }
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
