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

#include <algorithm>
#include <curl/curl.h>
#include <stdexcept>
#include <string>

using std::string;
namespace
{
constexpr auto HACKERNEWS_API_URL{"https://hacker-news.firebaseio.com/v0/"};
constexpr auto FETCH_ERROR_MSG{"Failed to parse Response to json"};
} // namespace

Hackernews::~Hackernews()
{
    curl_easy_cleanup(_curl);
}

HnEntry Hackernews::getEntry(int entryID)
{
    const auto j{get("item/" + std::to_string(entryID) + ".json")};
    HnEntry entry;

    if (j["id"].isNumeric())
    {
        entry.id = j.get("id", 0).asInt();
    }

    if (j["by"].isString())
    {
        entry.by = j["by"].asString();
    }

    if (j["deleted"].isBool())
    {
        entry.deleted = j["deleted"].asBool();
    }

    if (j["dead"].isBool())
    {
        entry.flagged = j["dead"].asBool();
    }

    if (j["time"].isNumeric())
    {
        entry.time = j["time"].asInt();
    }

    if (j["descendants"].isNumeric())
    {
        entry.descendants = j["descendants"].asInt();
    }

    if (j["parent"].isNumeric())
    {
        entry.parent = j["parent"].asInt();
    }

    if (j["score"].isNumeric())
    {
        entry.score = j["score"].asInt();
    }

    if (j["kids"].isArray())
    {
        const auto in{j["kids"]};
        entry.kids.reserve(in.size());
        std::transform(in.begin(), in.end(), std::back_inserter(entry.kids), [](const auto &e) { return e.asInt(); });
    }

    if (j["text"].isString())
    {
        entry.text = j["text"].asString();
    }

    if (j["title"].isString())
    {
        entry.title = j["title"].asString();
    }

    return entry;
}

HnUser Hackernews::getUser(const string &username)
{
    const auto j{get("user/" + username + ".json")};

    auto user {HnUser()};

    if (j["id"].isString())
    {
        user.id = j["id"].asString();
    }
    if (j["about"].isString())
    {
        user.about = j["about"].asString();
    }
    if (j["created"].isNumeric())
    {
        user.created = j["created"].asInt();
    }
    if (j["karma"].isNumeric())
    {
        user.karma = j["karma"].asInt();
    }

    if (j["submitted"].isArray())
    {
        const auto in{j["submitted"]};

        user.submitted.reserve(in.size());
        std::transform(in.begin(), in.end(), std::back_inserter(user.submitted),
                       [](const auto &e) { return e.asInt(); });
    }

    return user;
}

Json::Value Hackernews::get(const string &apiEndpoint)
{
  const auto url{HACKERNEWS_API_URL + apiEndpoint};

  string readBuffer;
  CURLcode res;

  if (_curl == nullptr) {
    Util::connectToNetwork();
    _curl = curl_easy_init();
  }

  if (_curl)
  {
    struct curl_slist *headers = NULL;
    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(_curl);

    if (res == CURLE_OK)
    {
      long response_code;
      curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &response_code);

      switch (response_code)
      {
        case 200: {
          Json::Value root;
          Json::Reader reader;
          if (reader.parse(readBuffer, root))
          {
            return root;
          }
          _logger.writeErrorLog(FETCH_ERROR_MSG);
          throw std::runtime_error(FETCH_ERROR_MSG);
        }
        default: {
          _logger.writeErrorLog("Hackernews API: " + url + " Response Code: " + std::to_string(res));
          throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
        }
      }
    }
    else if (res == CURLE_OPERATION_TIMEDOUT)
    {
      throw std::runtime_error("Firebaseio.com is blocked on some DNS. The Hackernews API therefore cannot be "
                               "served. (Curl Error Code " +
                               std::to_string(res) + ")");
    }
    else
    {
      throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
    }
  }
  throw::std::runtime_error("Curl node not set.");
}
