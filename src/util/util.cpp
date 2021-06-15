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

string Util::readFromConfig(const string &name)
{
    iconfigedit *temp = nullptr;
    iconfig *config = OpenConfig(CONFIG_PATH.c_str(), temp);
    string url = ReadString(config, name.c_str(), "");
    CloseConfigNoSave(config);
    return url;
}