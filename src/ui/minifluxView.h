//------------------------------------------------------------------
// listView.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An UI class to display items in a listview
//-------------------------------------------------------------------

#ifndef MINIFLUXVIEW
#define MINIFLUXVIEW

#include "minifluxModel.h"
#include "listView.h"
#include "minifluxViewEntry.h"

#include <vector>
#include <memory>

class MinifluxView final : public ListView
{
public:
    /**
        * Displays a list view
        *
        * @param ContentRect area of the screen where the list view is placed
        * @param Items items that shall be shown in the listview
        * @param page page that is shown, default is 1
        */
    MinifluxView(const irect &contentRect, const std::vector<MfEntry> &mfEntries, int page = 1);

    std::vector<int> getEntriesTillPage();

    MfEntry& getCurrentEntry() { return getEntry(_selectedEntry); };

    MfEntry& getEntry(int entryID) {return std::dynamic_pointer_cast<MinifluxViewEntry>(_entries.at(entryID))->getMfEntry(); };

};
#endif
