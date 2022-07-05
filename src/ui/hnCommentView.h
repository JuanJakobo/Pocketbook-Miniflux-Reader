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

#include <memory>
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
    HnCommentView(const irect *contentRect, std::vector<HnEntry> *readerentries, int page = 1);

    HnEntry *getCurrentEntry() {
        return getEntry(_selectedEntry);
    }

    HnEntry *getEntry(int entryID) {
        return std::dynamic_pointer_cast<HnCommentViewEntry>(_entries.at(entryID))->get();
    };
};
#endif
