//------------------------------------------------------------------
// hnCommentView.h
//
// Author:           JuanJakobo
// Date:             04.08.2020
// Description:      An UI class to display items in a listview
//-------------------------------------------------------------------

#ifndef HNCOMMENTVIEW
#define HNCOMMENTVIEW

#include "inkview.h"
#include "hnCommentViewEntry.h"
#include "item.h"

#include <vector>

class HnCommentView
{
public:
    /**
        * Displays a list view 
        * 
        * @param ContentRect area of the screen where the list view is placed
        * @param Items items that shall be shown in the listview
        */
       //const?
    HnCommentView(const irect *contentRect, std::vector<hnItem> *readerentries, int page =1);

    ~HnCommentView();

    const int getEntryFontHeight() { return _entryFontHeight; };
    ifont *getEntryFont() { return _entryFont; };
    ifont *getEntryFontBold() { return _entryFontBold; };

    void draw();

    /**
        * Draws an single entry to the screen
        * 
        * @param itemID the id of the item that shall be drawn
        */
    void drawEntry(int itemID);

    /**
        * inverts the color of an entry 
        * 
        * @param itemID the id of the item that shall be inverted
        */
    void invertEntryColor(int itemID);

    /**
        * Iterates through the items and sends them to the listViewEntry Class for drawing
        */
    void drawEntries();

    /**
        * Navigates to the next page
        */
    void nextPage() { this->actualizePage(_shownPage + 1); };

    /**
        * Navigates to the prev page
        */
    void prevPage() { this->actualizePage(_shownPage - 1); };

    /**
        * Navigates to first page
        */
    void firstPage() { this->actualizePage(1); };

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
    int _footerFontHeight;
    int _entryFontHeight;
    const irect *_contentRect;
    std::vector<HnCommentViewEntry> _entries;
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

    /**
        * Draws the footer including a page changer 
        */
    void drawFooter();

    /**
        * updates an entry 
        * 
        * @param itemID the id of the item that shall be inverted
        */
    void updateEntry(int itemID);

    /**
        * Navigates to the selected page
        * 
        * @param pageToShown page that shall be shown
        */
    void actualizePage(int pageToShown);
};
#endif