//------------------------------------------------------------------
// miniflux.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "miniflux.h"
#include "util.h"
#include "item.h"
#include "log.h"
#include <nlohmann/json.hpp>

#include <string>
#include <curl/curl.h>

Miniflux::Miniflux()
{
    if (iv_access(MINIFLUX_PATH.c_str(), W_OK) != 0)
        iv_mkdir(MINIFLUX_PATH.c_str(), 0777);
}

bool Miniflux::setItems(const vector<Item> &tempItems)
{
    if (tempItems.empty())
        return false;

    if (!_items.empty())
        _items.clear();

    _items = tempItems;

    return true;
}

bool Miniflux::login()
{
    string tempPath = getUrl();
    if (setItems(getDataStructure(tempPath)))
    {
        _loggedIn = true;
        return true;
    }

    return false;
}

vector<Item> Miniflux::getDataStructure(string &pathUrl)
{
    return getDataStructure(pathUrl, this->getToken());
}

vector<Item> Miniflux::getDataStructure(const string &pathUrl, const string &token)
{

    if (!Util::connectToNetwork())
    {
        Message(ICON_WARNING, "Warning", "Cannot connect to the internet. Switching to offline modus. To work online turn on online modus in the menu.", 2000);
        _workOffline = true;
    }

    if (_url.empty())
        _url = this->getUrl();

    if (token.empty())
    {
        Message(ICON_ERROR, "Error", "Token not set.", 2000);
        return {};
    }

    Log::writeLog("Starting download of DataStructure");
    //here get entries
    string url = _url + "/v1/entries?status=unread&direction=desc";
    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, ("X-Auth-Token: " +  token).c_str());
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
                auto j = nlohmann::json::parse(readBuffer);
                //std::cout << j["total"].dump(4) << std::endl;
                
                vector<Item> tempItems;

                for (auto &element : j["entries"].items())
                {
                    tempItems.push_back(Item(element.value()["id"],
                                            element.value()["title"],
                                            element.value()["url"],
                                            element.value()["comments_url"],
                                            "TEST")); //element.value()["content"]));
                }

                return tempItems;
            }
            default:
                Message(ICON_ERROR, "Error", ("An unknown error occured. (Curl Response Code " + std::to_string(response_code) + ")").c_str(), 4000);
                _workOffline = true;
            }
        }
        else
        {
            Message(ICON_ERROR, "Error", ("An curl error occured (Error Code: " + std::to_string(res) + ").").c_str(), 4000);
        }
    }
    return {};
}

string Miniflux::getUrl()
{
    iconfigedit *temp = nullptr;
    iconfig *minifluxConfig = OpenConfig(MINIFLUX_CONFIG_PATH.c_str(), temp);
    string url = ReadString(minifluxConfig, "url", "");
    CloseConfigNoSave(minifluxConfig);
    return url;
}

string Miniflux::getToken()
{
    iconfigedit *temp = nullptr;
    iconfig *minifluxConfig = OpenConfig(MINIFLUX_CONFIG_PATH.c_str(), temp);
    string token = ReadString(minifluxConfig, "token", "");
    CloseConfigNoSave(minifluxConfig);
    return token;
}