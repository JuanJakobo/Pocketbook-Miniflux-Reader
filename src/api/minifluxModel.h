//------------------------------------------------------------------
// MinifluxModel.h
//
// Author:           JuanJakobo
// Date:             23.04.2021
// Description:      
//-------------------------------------------------------------------

#ifndef MINIFLUXMODEL
#define MINIFLUXMODEL

#include "model.h"

#include <string>

enum IsDownloaded
{
				NOTDOWNLOADED,
				TOBEDOWNLOADED,
				DOWNLOADED
};

struct MfEntry : Entry
{
  std::string status;
  std::string title;
  std::string url;
  std::string comments_url;
  std::string content;
  bool starred;
  int reading_time; //in min
  IsDownloaded downloaded = IsDownloaded::NOTDOWNLOADED;
};

struct MfFeedIcon
{
  int id;
  std::string data;
  std::string mime_type;
};

#endif
