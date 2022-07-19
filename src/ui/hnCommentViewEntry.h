//------------------------------------------------------------------
// hnCommentViewEntry.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An listViewEntry that handles an item of a listview
//-------------------------------------------------------------------

#ifndef HNCOMMENTVIEWENTRY
#define HNCOMMENTVIEWENTRY

#include "listViewEntry.h"
#include "hackernewsModel.h"

class HnCommentViewEntry final : public ListViewEntry
{
public:
    /**
        * Creates an ListViewEntry
        *
        * @param Page site of the listView the Entry is shown
        * @param Rect area of the screen the item is positioned
        * @param entry entry that shall be drawn
        */
    HnCommentViewEntry(int page, const irect position, const HnEntry &entry, bool drawHeader);

    /**
        * draws the listViewEntry to the screen
        *
        * @param entryFont font for the entry itself
        * @param entryFontBold bold font for the header
        * @param fontHeight height of the font
        */
    void draw(const ifont *entryFont, const ifont *entryFontBold, int fontHeight) override;

    HnEntry *get() override { return &_entry; };

private:
    HnEntry _entry;
    bool _drawHeader;
};
#endif
