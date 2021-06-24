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

struct entry
{
  int id;
  std::string status;
  std::string title;
  std::string url;
  std::string comments_url;
  std::string content;
  bool starred;
  int reading_time; //in min
};

struct feedIcon
{
  int id;
  std::string data;
  std::string mime_type;
};

#endif