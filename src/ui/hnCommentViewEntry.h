//------------------------------------------------------------------
// hnCommentViewEntry.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An listViewEntry that handles an item of a listview
//-------------------------------------------------------------------

#ifndef HNCOMMENTVIEWENTRY
#define HNCOMMENTVIEWENTRY

#include "inkview.h"
#include "item.h"

class HnCommentViewEntry
{
public:
    /**
        * Creates an ListViewEntry 
        * 
        * @param Page site of the listView the Entry is shown
        * @param Rect area of the screen the item is positioned
        * @param entry entry that shall be drawn
        */
    HnCommentViewEntry(int page, irect position, hnItem *entry);

    hnItem *getEntry() { return _entry; }
    //const hnItem *getEntry() { return &_entry; }
    irect *getPosition() { return &_position; }
    int getPage() const { return _page; }

    /**
        * draws the listViewEntry to the screen
        * 
        * @param entryFont font for the entry itself
        * @param entryFontBold bold font for the header
        * @param fontHeight height of the font 
        */
    void draw(ifont *entryFont, ifont *entryFontBold, int fontHeight);

private:
    hnItem *_entry;
    int _page;
    irect _position;
};
#endif