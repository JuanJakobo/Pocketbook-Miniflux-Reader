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
    nlohmann::json j = get("/v1/entries?" + filter);

    vector<entry> tempItems;

    if (j.empty())
    {
        //TODO ERROR
    }

    for (auto &element : j["entries"].items())
    {
        //TODO test if is null
        //if(element.value()["content"].empty())
        //{
        //Log::writeLog("Empty Content on " + element.value()["title"]);
        //}
        //download also feed info
        tempItems.push_back({element.value()["id"],
                             //element.value()["feed_id"],
                             element.value()["status"], //read unread
                             element.value()["title"],
                             element.value()["url"],
                             element.value()["comments_url"],
                             //element.value()["published_at"],
                             element.value()["content"],
                             element.value()["starred"],
                             element.value()["reading_time"]});
    }
    //TODO where to handle this case?
    //if (tempItems.empty())
    //    throw std::runtime_error("Error"); //TODO catch
    return tempItems;
}

//TODO return bitmap
//vector<entry> Miniflux::getFeedIcon(const string &feedID)
//{
//    nlohmann::json j = get("/v1/feeds"+ feedID + "/icon");

//{
//"id": 262,
//"data": "image/png;base64,iVBORw0KGgoAAA....",
//"mime_type": "image/png"
//}

//returns 404 if no icon exists

//    return {};        //TODO test if is null

//}

//updateFeed();
//updateEntries();

// TODO MAKE for put

bool Miniflux::markUserEntriesAsRead(int userID)
{
    string url = _url + "/v1/users/" + std::to_string(userID) + "/mark-all-as-read";
    Log::writeLog(url);

    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {
        //TODO put header?
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("X-Auth-Token: " + _token).c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        //TODO in other class
        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            switch (response_code)
            {
            case 204:
                return true;
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

bool Miniflux::updateEntries(const vector<entry> &entrys) //todo add actions?
{
    string url = _url + "/v1/entries";
    Log::writeLog(url);

    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    struct WriteThis wt;

    //only can change status read /unread...
    string data = "{\"entry_ids\": [6655, 6646], \"status\": \"unread\"}"; //\"starred\": \"true\"}";

    if (curl)
    {
        //TODO put header?
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("X-Auth-Token: " + _token).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        //curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        //curl_easy_setopt(curl, CURLOPT_READDATA, data.c_str());
        //curl_easy_setopt(curl, CURLOPT_READFUNCTION, Util::read_callback);
        //curl_easy_setopt(curl, CURLOPT_READDATA, &wt);
        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        //TODO in other class
        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            Log::writeLog("test3");
            switch (response_code)
            {
            case 204:
                return true;
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

    //TODO test if url and token are empty

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

        //TODO in other class
        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            //TODO catch further responses
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
