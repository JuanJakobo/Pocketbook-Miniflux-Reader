//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------
#include "util.h"

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
//also throw errors
bool Util::connectToNetwork()
{
    iv_netinfo *netinfo = NetInfo();
    if (netinfo->connected)
        return true;

    const char *network_name = nullptr;
    int result = NetConnect2(network_name, 1);
    if (result != 0)
    {
        return false;
    }

    netinfo = NetInfo();
    if (netinfo->connected)
        return true;

    return false;
}

//int or string? do a template?
string Util::readFromConfig(string name)
{
    iconfigedit *temp = nullptr;
    iconfig *config = OpenConfig(CONFIG_PATH.c_str(), temp);
    string url = ReadString(config, name.c_str(), "");
    CloseConfigNoSave(config);
    return url;
}