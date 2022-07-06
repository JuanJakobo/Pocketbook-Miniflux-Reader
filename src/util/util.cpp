//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------
#include "util.h"
#include "inkview.h"
#include "eventHandler.h"
#include "log.h"

#include <string>
#include <iostream>
#include <fstream>

using std::string;

size_t Util::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

size_t Util::writeData(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = iv_fwrite(ptr, size, nmemb, stream);
    return written;
}

//https://github.com/pmartin/pocketbook-demo/blob/master/devutils/wifi.cpp
void Util::connectToNetwork()
{
    //NetError, NetErrorMessage
    iv_netinfo *netinfo = NetInfo();
    if (netinfo->connected){
        ShowHourglassForce();
        return;
    }

    const char *network_name = nullptr;
    int result = NetConnect2(network_name, 1);
    if (result != 0)
        throw std::runtime_error("Could not connect to the internet.");

    netinfo = NetInfo();
    if (netinfo->connected){
        ShowHourglassForce();
        return;
    }

    throw std::runtime_error("Could not connect to the internet.");
}

string Util::accessConfig(const Action &action, const string &name, const string &value)
{
    iconfigedit *temp = nullptr;
    iconfig *config = OpenConfig(CONFIG_PATH.c_str(), temp);
    string returnValue;

    switch (action)
    {
    case Action::IWriteSecret:
        if (!value.empty())
            WriteSecret(config, name.c_str(), value.c_str());
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
    CURL *curl = curl_easy_init();

    Util::connectToNetwork();

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
                return readBuffer;
            default:
                throw std::runtime_error("getData " + url + " HTML Error Code" + std::to_string(response_code));
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
    replaceAll(data, "&t;", "<");
    replaceAll(data, "&gt;", ">");
    //html
    replaceAll(data, "&#x27;", "\'");
    replaceAll(data, "&#x2F;", "/");
    replaceAll(data, "<p>", "\n");
    replaceAll(data, "<i>", "\"");
    replaceAll(data, "</i>", "\"");
}

string Util::clearString(string title)
{
    const std::string forbiddenInFiles = "<>\\/:?\"|";
    std::transform(title.begin(), title.end(), title.begin(), [&forbiddenInFiles](char c)
            { return forbiddenInFiles.find(c) != std::string::npos ? ' ' : c; });
    return title;
}

string Util::createHtml(string title, string content)
{

    title = clearString(title);

    string path = ARTICLE_FOLDER + "/" + title + ".html";
    if (iv_access(path.c_str(), W_OK) != 0)
    {
        string result = content;

        auto found = content.find("<img");
        auto counter = 0;
        while (found != std::string::npos)
        {
            auto imageFolder = "img/" + title;

            if (iv_access((ARTICLE_FOLDER + "/" + imageFolder).c_str(), W_OK) != 0)
                iv_mkdir((ARTICLE_FOLDER + "/" + imageFolder).c_str(), 0777);

            auto imagePath = imageFolder + "/" + std::to_string(counter);

            content = content.substr(found);
            auto src = content.find("src=\"");
            content = content.substr(src + 5);
            auto end = content.find("\"");
            auto imageURL = content.substr(0, end);

            if (iv_access((ARTICLE_FOLDER + "/" + imagePath).c_str(), W_OK) != 0)
            {
                try
                {
                    std::ofstream img;
                    img.open(ARTICLE_FOLDER + "/" + imagePath);
                    img << Util::getData(imageURL);
                    img.close();
                }
                catch (const std::exception &e)
                {
                    Log::writeErrorLog(e.what());
                }

                auto toReplace = result.find(imageURL);

                if (toReplace != std::string::npos)
                    result.replace(toReplace, imageURL.length(), imagePath);
            }
            counter++;
            found = content.find("<img");
        }

        std::ofstream htmlfile;
        htmlfile.open(path);
        htmlfile << result;
        htmlfile.close();
    }
    return path;
}

void Util::replaceAll(std::string &data, const std::string &replace, const std::string &by)
{
    auto start = 0;
    while ((start = data.find(replace, start)) != std::string::npos)
    {
        data.replace(start, replace.size(), by);
        start += by.length();
    }
}
