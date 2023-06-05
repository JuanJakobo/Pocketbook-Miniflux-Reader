//------------------------------------------------------------------
// pocket.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "pocket.h"
#include "eventHandler.h"
#include "log.h"
#include "pocketModel.h"
#include "util.h"

#include <curl/curl.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace
{
    using namespace std::string_literals;
    const auto POCKET_URL{"https://getpocket.com/v3/"s};
    const auto CONSUMER_KEY{"<Consumer-Key>"s};
} // namespace

Pocket::Pocket()
{
    if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
        _accessToken = Util::accessConfig(Action::IReadSecret, "AccessToken");

    if (_accessToken.empty())
        loginDialog();
}

void Pocket::loginDialog()
{
    try
    {
        auto code = getCode();
        auto dialogResult =
            DialogSynchro(ICON_QUESTION, "Action",
                          ("Please type the URL below into your browser to grant this application access to Pocket. "
                           "After you accepted please click done.(It is normal that, once you accepted, the page fails "
                           "to load) \n https://getpocket.com/auth/authorize?request_token=" +
                           code)
                              .c_str(),
                          "Done", "Cancel", NULL);
        switch (dialogResult)
        {
        case 1: {
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

string Pocket::getCode()
{
    const auto root{
        post("oauth/request", "{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"redirect_uri\":\"github.com\"}")};
    if (!root["code"].isString())
    {
        throw std::runtime_error("could not receive code.");
    }
    return root["code"].asString();
}

void Pocket::getAccessToken(const string &code)
{
    const auto root = post("oauth/authorize", "{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"code\":\"" + code + "\"}");
    if (root["username"].isString())
    {
        // TODO compare
        Util::accessConfig(Action::IWriteSecret, "Username", root["username"].asString());
    }

    if (root["access_token"].isString())
    {
        _accessToken = root["access_token"].asString();
        Util::accessConfig(Action::IWriteSecret, "AccessToken", _accessToken);
    }
    else
    {
        throw std::runtime_error("could not receive authentifcation token.");
    }
}

void Pocket::addItems(const string &url)
{
    // TODO curl url-encode url
    string postData = "{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"access_token\":\"" + _accessToken +
                      "\",\"url\":\"" + url + "\"}";
    const auto root = post("add", postData);
    // TODO WHAT TO DO WITH IT?
}


//TODO search for items
vector<PocketItem> Pocket::getItems()
{
    string postData = "{\"consumer_key\":\"" + CONSUMER_KEY + "\", \"access_token\":\"" + _accessToken +
                      "\",\"detailType\":\"simple\",\"contentType\":\"article\",\"state\":\"all\"";
    auto lastUpdate = Util::accessConfig(Action::IReadString, "lastUpdate");
    if (!lastUpdate.empty())
    {
        postData += ",\"since\":" + lastUpdate;
    }
    postData += '}';

    auto const j = post("get", postData);

    if (j["since"].isNumeric())
    {
        const auto since = j["since"].asString();
        Util::accessConfig(Action::IWriteString, "lastUpdate", since);
    }

    vector<PocketItem> tempItems;
    // TODO size is known
    for (const auto &element : j["list"])
    {
        PocketItem temp;
        if (element["item_id"].isString())
        {
            temp.id = element["item_id"].asInt();
        }
        if (element["given_url"].isString())
        {
            temp.url = element["given_url"].asString();
        }
        if (element.isMember("favorite"))
        {
            temp.starred = (element["favorite"].asString() == "0");
        }
        if (element.isMember("status"))
        {
            // TODO get it once...
            if (element["status"].asString() == "0")
            {
                temp.status = IStatus::UNREAD;
            }
            else if (element["status"].asString() == "1")
            {
                temp.status = IStatus::ARCHIVED;
            }
            else if (element["status"].asString() == "2")
            {
                temp.status = IStatus::TODELETE;
            }
        }
        if (element["resolved_title"].isString())
        {
            temp.title = element["resolved_title"].asString();
            temp.path = ARTICLE_FOLDER + "/" + Util::clearString(temp.title) + ".html";
        }
        if (element["excerpt"].isString())
        {
            temp.excerpt = element["excerpt"].asString();
        }
        if (element["time_to_read"].isNumeric())
        {
            temp.reading_time = element["time_to_read"].asInt();
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
    switch (action)
    {
    case PocketAction::ADD:
        stAction = "add";
        break;
    case PocketAction::ARCHIVE:
        stAction = "archive";
        break;
    case PocketAction::READD:
        stAction = "readd";
        break;
    case PocketAction::FAVORITE:
        stAction = "favorite";
        break;
    case PocketAction::UNFAVORITE:
        stAction = "unfavorite";
        break;
    case PocketAction::DELETE:
        stAction = "delete";
        break;
    }
    return stAction;
}

void Pocket::sendItem(PocketAction action, const string &id)
{
    std::string postData =
        "{\"consumer_key\":\"" + CONSUMER_KEY + "\",\"access_token\":\"" + _accessToken + "\",\"actions\":[";
    postData += "{\"action\":\"" + determineAction(action) + "\",\"item_id\":\"" + id + "\"}]}";
    auto const j = post("send", postData);
}

void Pocket::sendItems(PocketAction action, const vector<string> &ids)
{

    std::string postData =
        "{\"consumer_key\":\"" + CONSUMER_KEY + "\",\"access_token\":\"" + _accessToken + "\",\"actions\":[";
    string stAction = determineAction(action);
    auto comma{false};
    for (auto id : ids)
    {
        if (comma)
        {
            postData += ',';
        }
        postData += "{\"action\":\"" + stAction + "\",\"item_id\":\"" + id + "\"}";
        if (!comma)
        {
            comma = true;
        }
    }
    postData += "]}";
    auto const j = post("send", postData);
}

Json::Value Pocket::post(const string &apiEndpoint, const string &data)
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
            case 400:
                throw std::runtime_error(
                    "Invalid request, please make sure you follow the documentation for proper syntax");
            case 401:
                throw std::runtime_error("Problem authenticating the user");
            case 403:
                throw std::runtime_error(
                    "User was authenticated, but access denied due to lack of permission or rate limiting.");
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
