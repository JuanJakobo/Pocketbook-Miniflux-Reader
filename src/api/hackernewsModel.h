//------------------------------------------------------------------
// hackernewsModel.h
//
// Author:           JuanJakobo
// Date:             31.05.2021
// Description: Model of HnCommentViewEntry
//-------------------------------------------------------------------

#ifndef HACKERNEWSMODEL
#define HACKERNEWSMODEL

#include "model.h"

#include <string>
#include <vector>

struct HnEntry final : Entry
{
    std::string by;
    int time; //unix timestamp
    bool deleted = false;
    bool flagged = false;
    std::string text;
    int parent = 0;
    std::vector<int> kids;
    std::vector<std::string> urls;
    int score = 0;
    std::string title;
    int descendants = 0;
    int mfEntryId;
};

struct HnUser
{
    std::string id;
    std::string about;
    int created; //unix timestamp
    int karma;
    std::vector<int> submitted;
};
#endif
