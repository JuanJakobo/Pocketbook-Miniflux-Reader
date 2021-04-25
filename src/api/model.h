//------------------------------------------------------------------
// model.h
//
// Author:           JuanJakobo
// Date:             23.04.2021
// Description:      
//-------------------------------------------------------------------

#ifndef MODEL
#define MODEL

#include <string>

using std::string;

struct entry
{
  int id;
  string title;
  string url;
  string comments_url;
  string content;
};
#endif