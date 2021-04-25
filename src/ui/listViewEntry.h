//------------------------------------------------------------------
// listViewEntry.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An listViewEntry that handles an item of a listview
//-------------------------------------------------------------------

#ifndef LISTVIEWENTRY
#define LISTVIEWENTRY

#include "inkview.h"
#include "model.h"

class ListViewEntry
{
public:
    /**
        * Creates an ListViewEntry 
        * 
        * @param Page site of the listView the Entry is shown
        * @param Rect area of the screen the item is positioned
        * @param entry entry that shall be drawn
        */
    ListViewEntry(int page, irect position, entry entry);

    entry *getEntry() {return &_entry;};
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
    entry _entry;
    int _page;
    irect _position;
};
#endif