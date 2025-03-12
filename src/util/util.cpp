//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------
#include "util.h"
#include "eventHandler.h"
#include "inkview.h"
#include "log.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <ostream>
#include <string>

#pragma GCC diagnostic ignored "-Wold-style-cast"

using std::string;

size_t Util::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

int Util::writeData(void *ptr, int size, int nmemb, FILE *stream)
{
    auto const written{iv_fwrite(ptr, size, nmemb, stream)};
    return written;
}

// https://github.com/pmartin/pocketbook-demo/blob/master/devutils/wifi.cpp
void Util::connectToNetwork()
{
    // NetError, NetErrorMessage
    auto netinfo {NetInfo()};
    if (netinfo->connected)
    {
        ShowHourglassForce();
        return;
    }

    const char *network_name = nullptr;
    auto const result{NetConnect2(network_name, 1)};
    if (result != 0)
    {
        throw std::runtime_error("Could not connect to the internet.");
    }

    netinfo = NetInfo();
    if (netinfo->connected)
    {
        ShowHourglassForce();
        return;
    }

    throw std::runtime_error("Could not connect to the internet.");
}

string Util::accessConfig(const Action &action, const string &name, const string &value)
{
    iconfigedit *temp = nullptr;
    const auto config{OpenConfig(CONFIG_PATH.c_str(), temp)};
    string returnValue;

    switch (action)
    {
    case Action::IWriteSecret:
        if (!value.empty())
        {
            WriteSecret(config, name.c_str(), value.c_str());
        }
        returnValue = {};
        break;
    case Action::IReadSecret:
        returnValue = ReadSecret(config, name.c_str(), "");
        break;
    case Action::IWriteString:
        if (!value.empty())
            WriteString(config, name.c_str(), value.c_str());
        returnValue = {};
        break;
    case Action::IReadString:
        returnValue = ReadString(config, name.c_str(), "");
        break;
    default:
        break;
    }
    CloseConfig(config);

    return returnValue;
}

string Util::getData(const string &url)
{
    string readBuffer;
    CURLcode res;
    auto const curl = curl_easy_init();

    if (curl)
    {

        struct curl_slist *headers = NULL;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Util::writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK)
        {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            switch (response_code)
            {
            case 200:
                return readBuffer;
            default:
                throw std::runtime_error("getData " + url + " HTML Error Code " + std::to_string(response_code));
            }
        }
        else
        {
            throw std::runtime_error("getData " + url + " Curl RES Error Code " + std::to_string(res));
        }
    }
    return {};
}

void Util::decodeHTML(string &data)
{
    replaceAll(data, "&quot;", "\"");
    replaceAll(data, "&amp;", "&");
    replaceAll(data, "&apos;", "\'");
    replaceAll(data, "&lt;", "<");
    replaceAll(data, "&gt;", ">");
    // html
    replaceAll(data, "&#x27;", "\'");
    replaceAll(data, "&#x2F;", "/");
    replaceAll(data, "<p>", "\n");
    replaceAll(data, "<i>", "\"");
    replaceAll(data, "</i>", "\"");
}

string Util::clearString(string title)
{
    const std::string forbiddenInFiles{"<>$\\/:?\"|*"};
    std::transform(title.begin(), title.end(), title.begin(),
                   [&forbiddenInFiles](char c) { return forbiddenInFiles.find(c) != std::string::npos ? '_' : c; });
    return title;
}

string Util::returnFolderName(string title)
{
    replaceAll(title, " ", "_");
    return ARTICLE_FOLDER + "/" + clearString(title) + "/";
}

string Util::returnTitlePath(string title)
{
    replaceAll(title, " ", "_");
    return returnFolderName(title) + clearString(title) + ".html";
}

bool Util::createHtml(const string &title, string content)
{
    const auto title_folder{returnFolderName(title)};
    const auto title_path{returnTitlePath(title)};
    const auto logger {Log("createHtml")};

    if (iv_access(title_folder.c_str(), W_OK) != 0)
    {
        Util::connectToNetwork();
        logger.writeInfoLog(title_folder);

        if (iv_mkdir(title_folder.c_str(), 0666) != 0)
        {
            logger.writeErrorLog("Could not create article path " + title_folder);
            return false;
        }

        auto finalHtml {content};

        std::map<string, int> images;
        auto counter = 0;

        auto found = content.find("<img");
        while (found != std::string::npos)
        {
            content = content.substr(found);
            auto it {content.find("src=\"")};
            if (it == std::string::npos)
                break;

            content = content.substr(it + 5);
            it = content.find("\"");
            if (it == std::string::npos)
                break;

            const auto ret{images.insert({content.substr(0, it), counter})};
            if (ret.second)
            {
                // TODO check for internet
                try
                {
                    const auto imagePath{title_folder + std::to_string(ret.first->second)};
                    std::ofstream img(imagePath, std::ofstream::out);
                    if (img.is_open())
                    {
                        // TODO check internet getting
                        img << Util::getData(ret.first->first);
                        img.close();
                    }
                    else
                    {
                        logger.writeErrorLog("Could not open image path " + imagePath);
                    }
                }
                catch (const std::exception &e)
                {
                    logger.writeErrorLog(e.what());
                }
                counter++;
            }

            const auto toReplace{finalHtml.find("src=\"" + ret.first->first)};

            if (toReplace != std::string::npos)
                finalHtml.replace(toReplace + 5, ret.first->first.length(), '/' + std::to_string(ret.first->second));
            found = content.find("<img");
        }

        std::ofstream htmlfile;
        htmlfile.open(title_path);
        htmlfile << finalHtml;
        htmlfile.close();
    }
    return true;
}

void Util::replaceAll(std::string &data, const std::string &replace, const std::string &by)
{
    size_t start{0};
    while ((start = data.find(replace, start)) != std::string::npos)
    {
        data.replace(start, replace.size(), by);
        start += by.length();
    }
}
