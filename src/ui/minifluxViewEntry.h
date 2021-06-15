//------------------------------------------------------------------
// minifluxViewEntry.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An MinifluxViewEntry that handles an item of a listview
//-------------------------------------------------------------------

#ifndef MINIFLUXVIEWENTRY
#define MINIFLUXVIEWENTRY

#include "listViewEntry.h"
#include "model.h"

#include <memory>

class MinifluxViewEntry : public ListViewEntry
{
public:
    /**
        * Creates an MinifluxViewEntry 
        * 
        * @param Page site of the listView the Entry is shown
        * @param Rect area of the screen the item is positioned
        * @param entry entry that shall be drawn
        */
    MinifluxViewEntry(int page, const irect &position, const entry &entry);

    /**
        * draws the MinifluxViewEntry to the screen
        * 
        * @param entryFont font for the entry itself
        * @param entryFontBold bold font for the header
        * @param fontHeight height of the font 
        */
    void draw(const ifont *entryFont, const ifont *entryFontBold, int fontHeight);

private:
    std::unique_ptr<entry> _entry;
};
#endif