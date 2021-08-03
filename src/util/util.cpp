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
void Util::connectToNetwork()
{
    //NetError, NetErrorMessage
    iv_netinfo *netinfo = NetInfo();
    if (netinfo->connected)
        return;

    const char *network_name = nullptr;
    int result = NetConnect2(network_name, 1);
    if (result != 0)
    {
        throw "Could not connect to internet.";
    }

    netinfo = NetInfo();
    if (netinfo->connected)
        return;
    throw "Could not connect to internet.";
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
                throw std::runtime_error("HTML Error Code" + std::to_string(response_code));
            }
        }
        else
        {
            throw std::runtime_error("Curl RES Error Code " + std::to_string(res));
        }
    }
    return {};
}

void Util::openInBrowser(const string &url)
{
    //TODO use browser --> in child??
    //string cmd = "exec /ebrmain/bin/webbrowser.sh www.google.de";
    //string cmd = "/ebrmain/bin/browser.app \"" + _minifluxView->getEntry(_tempItemID)->url + "\"";
    ///ebrmain/bin/webbrowser.sh "https://insideevs.com/news/514727/tesla-towing-70mph-fast-charging/"

    string cmd = "/ebrmain/bin/webbrowser.sh \"google.de\"";

    cmd = "/ebrmain/bin/browser.app \"google.de\"";

    //string cmd = "/ebrmain/bin/webbrowser.sh \"" + url + "\"";

    system(cmd.c_str());
    //execlp(cmd.c_str(), cmd.c_str(), (char *)NULL);
}