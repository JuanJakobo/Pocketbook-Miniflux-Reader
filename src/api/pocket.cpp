//------------------------------------------------------------------
// pocket.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "util.h"
#include "log.h"
#include "eventHandler.h"
#include "pocket.h"
#include "pocketModel.h"

#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using std::string;
using std::vector;

Pocket::Pocket()
{
    if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
        _accessToken = Util::accessConfig(Action::IReadSecret,"AccessToken");

    if(_accessToken.empty())
        loginDialog();
}

void Pocket::loginDialog(){
    try{
        auto code = getCode();
        auto dialogResult = DialogSynchro(ICON_QUESTION, "Action",("Please type the URL below into your browser to grant this application access to Pocket. After you accepted please click done.(It is normal that, once you accepted, the page fails to load) \n https://getpocket.com/auth/authorize?request_token=" + code).c_str(), "Done", "Cancel", NULL);
        switch (dialogResult)
        {
            case 1:
                {
                    getAccessToken(code);
                    break;
                }
            default:
                CloseApp();
                break;
        }
    }
    catch (const std::exception &e)
    {
        Log::writeErrorLog(e.what());
        Message(ICON_INFORMATION, "Error while logging in.", e.what(), 1200);
        loginDialog();
    }
}

string Pocket::getCode(){
    nlohmann::json j = post("oauth/request","{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"redirect_uri\":\"github.com\"}");
    if (!j["code"].is_string())
        throw std::runtime_error("could not receive code.");
    return j["code"];
}

void Pocket::getAccessToken(const string &code){
    nlohmann::json j = post("oauth/authorize","{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"code\":\"" + code  + "\"}");
    if (j["username"].is_string())
        Util::accessConfig(Action::IWriteSecret,"Username",j["username"]);
    if (j["access_token"].is_string()){
        _accessToken =  j["access_token"];
        Util::accessConfig(Action::IWriteSecret,"AccessToken",_accessToken);
    }
    else
    {
        throw std::runtime_error("could not receive authentifcation token.");
    }
}

void Pocket::addItems(const string &url)
{
    //TODO curl url-encode url
    string postData = "{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"access_token\":\"" + _accessToken  + "\",\"url\":\"" + url + "\"}";
    Log::writeInfoLog(postData);
    nlohmann::json j = post("add",postData);
}


//TODO search for items
vector<PocketItem> Pocket::getItems()
{
    string postData = "{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"access_token\":\"" + _accessToken  + "\",\"detailType\":\"simple\",\"contentType\":\"article\",\"state\":\"all\"";
    auto lastUpdate = Util::accessConfig(Action::IReadString,"lastUpdate");
    if(!lastUpdate.empty())
            postData += ",\"since\":" + lastUpdate;
    postData += '}';

    nlohmann::json j = post("get",postData);

    if(j["since"].is_number())
    {
        int since =j["since"];
        Util::accessConfig(Action::IWriteString,"lastUpdate",std::to_string(since));
    }

    vector<PocketItem> tempItems;
    for (const auto &element : j["list"].items()){
        PocketItem temp;
        if(element.value()["item_id"].is_string()){
            temp.id = element.value()["item_id"];
        }
        if(element.value()["given_url"].is_string())
            temp.url = element.value()["given_url"];
        if(!element.value()["favorite"].is_null())
        {
            if(element.value()["favorite"] == "0")
                temp.starred = false;
            else
                temp.starred = true;
        }
        if(!element.value()["status"].is_null())
        {
            if(element.value()["status"] == "0")
                temp.status = IStatus::UNREAD;
            else if(element.value()["status"] == "1")
                temp.status = IStatus::ARCHIVED;
            else if(element.value()["status"] == "2")
                temp.status = IStatus::TODELETE;
        }
        if(element.value()["resolved_title"].is_string())
        {
            temp.title = element.value()["resolved_title"];
            temp.path = ARTICLE_FOLDER + "/" + Util::clearString(temp.title) + ".html";
        }
        if(element.value()["excerpt"].is_string())
            temp.excerpt = element.value()["excerpt"];
        if(element.value()["time_to_read"].is_number()){
            temp.reading_time = element.value()["time_to_read"];
        }

        tempItems.push_back(temp);
    }

    return tempItems;

}

void Pocket::getText(PocketItem *item)
{
    //API ENDPOINT IS PRIVAT
}

string Pocket::determineAction(PocketAction action)
{
    string stAction;
    switch(action)
    {
        case ADD:
            stAction = "add";
            break;
        case ARCHIVE:
            stAction = "archive";
            break;
        case READD:
            stAction = "readd";
            break;
        case FAVORITE:
            stAction = "favorite";
            break;
        case UNFAVORITE:
            stAction = "unfavorite";
            break;
        case DELETE:
            stAction = "delete";
            break;
    }
    return stAction;
}

void Pocket::sendItem(PocketAction action, const string &id)
{
    //TODO test status
    //response does not help here as it is always the same
    std::string postData = "{\"consumer_key\":\"" + CONSUMER_KEY + "\",\"access_token\":\"" + _accessToken  + "\",\"actions\":[";
    postData += "{\"action\":\"" + determineAction(action) + "\",\"item_id\":\"" + id + "\"}]}";
    nlohmann::json j = post("send",postData);
}

void Pocket::sendItems(PocketAction action, const vector<string> &ids)
{

    std::string postData = "{\"consumer_key\":\"" + CONSUMER_KEY + "\",\"access_token\":\"" + _accessToken  + "\",\"actions\":[";
    string stAction =  determineAction(action);
    auto comma = false;
    for(auto id : ids)
    {
        if(comma)
            postData += ',';
        postData += "{\"action\":\"" + stAction + "\",\"item_id\":\"" + id + "\"}";
        if(!comma)
            comma = true;
    }
    postData += "]}";
    nlohmann::json j = post("send",postData);
}

nlohmann::json Pocket::post(const string &apiEndpoint, const string &data)
{
    Util::connectToNetwork();

    string url = POCKET_URL + apiEndpoint;

    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (curl)
    {

        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json; charset=UTF-8");
        headers = curl_slist_append(headers, "X-Accept: application/json");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

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
            case 400:
                throw std::runtime_error("Invalid request, please make sure you follow the documentation for proper syntax");
            case 401:
                throw std::runtime_error("Problem authenticating the user");
            case 403:
                throw std::runtime_error("User was authenticated, but access denied due to lack of permission or rate limiting.");
            case 503:
                throw std::runtime_error("Pocket's sync server is down for scheduled maintenance.");
            default:
                throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
            }
        }
        else
        {
            Log::writeErrorLog("pocket API: " + url + " RES Error Code: " + std::to_string(res));
            throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
        }
    }
    return {};
}
