//------------------------------------------------------------------
// miniflux.h
//
// Author:           JuanJakobo
// Date:             22.04.2021
// Description:      
//
//-------------------------------------------------------------------

#ifndef MINIFLUX
#define MINIFLUX

#include "model.h"
#include "util.h"
#include "log.h"

#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using std::vector;

class Miniflux
{
public:
    Miniflux(const string &url, const string &token);

    vector<entry> getEntries(const string &filter);


private:
    string _url;
    string _token;

    nlohmann::json get(const string &api);
};

#endif