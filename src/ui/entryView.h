//------------------------------------------------------------------
// EntryView.h
//
// Author:           JuanJakobo
// Date:             25.04.2021
// Description:      an view that displays the content of an entry 
//-------------------------------------------------------------------

#ifndef ENTRYVIEW
#define ENTRYVIEW 

#include "inkview.h"
#include "model.h"

class EntryView
{
public:
    /**
        * Creates an EntryView 
        * 
        * @param entry entry item to display
        * @param contentRect screen area where the content shall be displayed
        */
    EntryView(const entry *entry, const irect *contentRect);
    
    /**
        * Draws the EntryView to the screen
        * 
        * @param entryFont font for the entry itself
        * @param entryFontBold bold font for the header
        * @param fontHeight height of the font 
        */
    void draw(ifont *entryFont, ifont *entryFontBold, int fontHeight);

private:
    const entry *_entry;
    const irect *_contentRect;
};
#endif