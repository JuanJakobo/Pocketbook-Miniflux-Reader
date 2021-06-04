//------------------------------------------------------------------
// listView.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An UI class to display items in a listview
//-------------------------------------------------------------------

#ifndef LISTVIEW
#define LISTVIEW

#include "inkview.h"
#include "listViewEntry.h"
#include "model.h"

#include <vector>

const int ITEMS_PER_PAGE = 7;

class ListView
{
public:
    /**
        * Displays a list view 
        * 
        * @param ContentRect area of the screen where the list view is placed
        * @param Items items that shall be shown in the listview
        */
    ListView(const irect *contentRect, const std::vector<entry> readerentries);

    ~ListView();

    //TODO if entry is out of bounds throw error
    const entry *getEntry(int itemID) { return &_readerentries[itemID]; };

    //TODO --> where do I handle fonts? in Util?

    //temp static
    const int getEntryFontHeight() { return _entryFontHeight; };
    ifont *getEntryFont() { return _entryFont; };
    ifont *getEntryFontBold() { return _entryFontBold; };

    void draw();

    /**
        * Draws the header
        * 
        * @param headerText the text that shall be displayed in the header
        * 
        */
    void drawHeader(string headerText);

    /**
        * Draws the footer including a page changer 
        */
    void drawFooter();

    /**
        * Draws an single entry to the screen
        * 
        * @param itemID the id of the item that shall be drawn
        */
    void drawEntry(int itemID);

    /**
        * Iterates through the items and sends them to the listViewEntry Class for drawing
        */
    void drawEntries();

    /**
        * Navigates to the selected page
        * 
        * @param pageToShown page that shall be shown
        */
    void actualizePage(int pageToShown);

    /**
        * Checkes if the listview has been clicked and either changes the page or returns item ID
        * 
        * @param x x-coordinate
        * @param y y-coordinate
        * @return int Item ID that has been clicked, -1 if no Item was clicked
        */
    int listClicked(int x, int y);

private:
    int _footerHeight;
    int _headerHeight;
    int _headerFontHeight;
    int _footerFontHeight;
    int _entryFontHeight;
    const irect *_contentRect;
    const std::vector<entry> _readerentries;
    std::vector<ListViewEntry> _entries;
    ifont *_footerFont;
    ifont *_entryFont;
    ifont *_entryFontBold;
    int _page;
    int _shownPage;
    irect _pageIcon;
    irect _nextPageButton;
    irect _prevPageButton;
    irect _firstPageButton;
    irect _lastPageButton;
};
#endif