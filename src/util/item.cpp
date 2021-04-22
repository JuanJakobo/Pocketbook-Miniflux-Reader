//------------------------------------------------------------------
// item.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "item.h"
#include "inkview.h"

#include <string>

using namespace std;
Item::Item(const int id, const string &title, const string &url, const string &comments_url, const string &content) : _id(id), _title(title), _url(url), _comments_url(comments_url), _content(content)
{

}