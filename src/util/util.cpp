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

#include <string>

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
bool Util::connectToNetwork()
{
    //NetError, NetErrorMessage
    iv_netinfo *netinfo = NetInfo();
    if (netinfo->connected){
        ShowHourglassForce();
        return true;
    }

    const char *network_name = nullptr;
    int result = NetConnect2(network_name, 1);
    if (result != 0)
    {
						Message(ICON_ERROR, "Network Error","Could not connect to the internet.", 5000);
						return false;
    }

    netinfo = NetInfo();
    if (netinfo->connected){
				ShowHourglassForce();
        return true;
    }
		Message(ICON_ERROR, "Network Error","Could not connect to the internet.", 5000);
		return false;
}

void Util::writeToConfig(const string &name, const string &value)
{ 
    iconfigedit *temp = nullptr;
    iconfig *config = OpenConfig(CONFIG_PATH.c_str(), temp);
    WriteString(config, name.c_str(), value.c_str());
    CloseConfig(config);
}

string Util::readFromConfig(const string &name)
{
    iconfigedit *temp = nullptr;
    iconfig *config = OpenConfig(CONFIG_PATH.c_str(), temp);
    string url = ReadString(config, name.c_str(), "");
    CloseConfigNoSave(config);
    return url;
}

string Util::getData(const string &url)
{
    string readBuffer;
    CURLcode res;
    CURL *curl = curl_easy_init();

    if (!Util::connectToNetwork())
        throw std::runtime_error("getData " + url + " Could not connect to the internet");

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

void Util::replaceAll(std::string &data, const std::string &replace, const std::string &by)
{
    auto start = 0;
    while ((start = data.find(replace, start)) != std::string::npos)
    {
        data.replace(start, replace.size(), by);
        start += by.length();
    }
}
