//------------------------------------------------------------------
// hnCommentView.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An UI class to display items in a listview
//-------------------------------------------------------------------

#ifndef HNCOMMENTVIEW
#define HNCOMMENTVIEW

#include "listView.h"
#include "hnCommentViewEntry.h"
#include "hackernewsModel.h"

#include <vector>

class HnCommentView final : public ListView
{
public:
    /**
        * Displays a list view
        *
        * @param ContentRect area of the screen where the list view is placed
        * @param Items items that shall be shown in the listview
        * @param page page that is shown, default is 1
        */
    HnCommentView(const irect &contentRect,const std::vector<HnEntry> &readerentries, int page = 1);

    HnEntry &getCurrentEntry() const { return getEntry(_selectedEntry); };
    HnEntry &getEntry(int entryID) const { return std::dynamic_pointer_cast<HnCommentViewEntry>(_entries.at(entryID))->getHnEntry(); };
};
#endif
