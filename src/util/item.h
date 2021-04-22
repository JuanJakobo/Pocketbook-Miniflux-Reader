//------------------------------------------------------------------
// item.h
//
// Author:           JuanJakobo
// Date:             22.04.2021 
// Description:      Describes an article
//-------------------------------------------------------------------

#ifndef ITEM
#define ITEM

#include "inkview.h"

#include <string>

using std::string;

class Item
{
public:
    /**
     * Creates a new item 
     * 
     */
    Item(const int id, const string &title, const string &url, const string &comments_url, const string &content);

    int getId() const { return _id;};
    string getTitle() const { return _title;}
    string getUrl() const { return _url;}
    string getCommentsUrl() const { return _comments_url;}
    string getContent() const { return _title;}

private:
    int _id;
    string _title;
    string _url;
    string _comments_url;
    string _content;
};
#endif