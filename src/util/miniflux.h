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

#include "inkview.h"
#include "item.h"

#include <string>
#include <vector>

using std::vector;

const string MINIFLUX_PATH = "/mnt/ext1/system/config/miniflux";
const string MINIFLUX_CONFIG_PATH = MINIFLUX_PATH + "/miniflux.cfg";

class Miniflux
{
public:
    explicit Miniflux();

    bool setItems(const vector<Item> &tempItems);

    const vector<Item> &getItems() const { return _items; };
    bool isLoggedIn() const { return _loggedIn; };
    bool isWorkOffline() const { return _workOffline; };
    void switchWorkOffline() { _workOffline = !_workOffline; };

    /**
        * Handles login by receiving userdat from config
        * 
        * @return true - login succeeded, false - login failed
        */
    bool login();

    /**
        * gets the dataStructure of the given URL and writes its WEBDAV items to the items vector, reads Userdata from configfile
        * 
        * @param pathUrl URL to get the dataStructure of
        * @return vector of items
        */
    vector<Item> getDataStructure(string &pathUrl);

    vector<Item> getDataStructure(const string &pathUrl,const string &token);
private:
    vector<Item> _items;
    bool _loggedIn{false};
    string _url;
    bool _workOffline{false};


    string getUrl();
    string getToken();
};

#endif