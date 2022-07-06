//------------------------------------------------------------------
// pocketModel.h
//
// Author:           JuanJakobo
// Date:             23.04.2021
// Description:      Describes the structure of an pocket item
//-------------------------------------------------------------------

#ifndef POCKETMODEL
#define POCKETMODEL

#include "model.h"

#include <string>

enum IStatus
{
    UNREAD,
    ARCHIVED,
    TODELETE
};

enum PIsDownloaded
{
				PNOTDOWNLOADED,
				PDOWNLOADED,
                PINVALID
};

struct PocketItem : Entry
{
    std::string title;
    IStatus  status;
    std::string url;
    std::string excerpt;
    std::string path;
    int reading_time; //in min
    bool starred;
    PIsDownloaded downloaded = PIsDownloaded::PNOTDOWNLOADED;
};

#endif
