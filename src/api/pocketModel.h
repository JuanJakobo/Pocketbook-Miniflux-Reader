#pragma once
//------------------------------------------------------------------
// pocketModel.h
//
// Author:           JuanJakobo
// Date:             23.04.2021
// Description:      Describes the structure of an pocket item
//-------------------------------------------------------------------
#include "model.h"

#include <string>

enum class IStatus
{
    UNREAD,
    ARCHIVED,
    TODELETE
};

enum class PIsDownloaded
{
    PNOTDOWNLOADED,
    PDOWNLOADED,
    PINVALID
};

struct PocketItem : Entry
{
    std::string title;
    IStatus status;
    std::string url;
    std::string excerpt;
    std::string path;
    int reading_time; // in min
    bool starred;
    PIsDownloaded downloaded{PIsDownloaded::PNOTDOWNLOADED};
};
