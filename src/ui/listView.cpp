//------------------------------------------------------------------
// listView.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "model.h"
#include "listView.h"
#include "listViewEntry.h"

#include <string>
#include <vector>

using std::string;
using std::vector;


ListView::ListView(const irect *contentRect, int page) : _contentRect(contentRect), _shownPage(page) 
{
    _entries.clear();
}

ListView::~ListView()
{
    CloseFont(_entryFont);
    CloseFont(_entryFontBold);
    CloseFont(_footerFont);
}


void ListView::draw()
{
    FillAreaRect(_contentRect, WHITE);
    drawEntries();
    drawFooter();
}

void ListView::drawEntry(int itemID)
{
    FillAreaRect(_entries[itemID]->getPosition(), WHITE);
    _entries[itemID]->draw(_entryFont, _entryFontBold, _entryFontHeight);
    updateEntry(itemID);
}

void ListView::invertEntryColor(int itemID)
{
    InvertAreaBW(_entries[itemID]->getPosition()->x, _entries[itemID]->getPosition()->y, _entries[itemID]->getPosition()->w, _entries[itemID]->getPosition()->h);
    updateEntry(itemID);
}

void ListView::drawEntries()
{
    for (unsigned int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i]->getPage() == _shownPage)
            _entries[i]->draw(_entryFont, _entryFontBold, _entryFontHeight);
    }
}

int ListView::listClicked(int x, int y)
{
    if (IsInRect(x, y, &_firstPageButton))
    {
        firstPage();
    }
    else if (IsInRect(x, y, &_nextPageButton))
    {   
        nextPage();
    }
    else if (IsInRect(x, y, &_prevPageButton))
    {
        prevPage();
    }
    else if (IsInRect(x, y, &_lastPageButton))
    {
        actualizePage(_page);
    }
    else
    {
        for (unsigned int i = 0; i < _entries.size(); i++)
        {
            if (_entries[i]->getPage() == _shownPage && IsInRect(x, y, _entries[i]->getPosition()) == 1)
            {
                return i;
            }
        }
    }
    return -1;
}

void ListView::drawFooter()
{
    SetFont(_footerFont, WHITE);
    string footer = std::to_string(_shownPage) + "/" + std::to_string(_page);
    FillAreaRect(&_pageIcon, BLACK);

    DrawTextRect2(&_pageIcon, footer.c_str());
    FillAreaRect(&_firstPageButton, BLACK);
    DrawTextRect2(&_firstPageButton, "First");
    FillAreaRect(&_prevPageButton, BLACK);
    DrawTextRect2(&_prevPageButton, "Prev");
    FillAreaRect(&_nextPageButton, BLACK);
    DrawTextRect2(&_nextPageButton, "Next");
    FillAreaRect(&_lastPageButton, BLACK);
    DrawTextRect2(&_lastPageButton, "Last");
}

void ListView::updateEntry(int itemID)
{
    PartialUpdate(_entries[itemID]->getPosition()->x, _entries[itemID]->getPosition()->y, _entries[itemID]->getPosition()->w, _entries[itemID]->getPosition()->h);
}

void ListView::actualizePage(int pageToShown)
{
    if (pageToShown > _page)
    {
        Message(ICON_INFORMATION, "Info", "You have reached the last page, to return to the first, please click \"first.\"", 1200);
    }
    else if (pageToShown < 1)
    {
        Message(ICON_INFORMATION, "Info", "You are already on the first page.", 1200);
    }
    else
    {
        _shownPage = pageToShown;
        FillArea(_contentRect->x, _contentRect->y, _contentRect->w, _contentRect->h, WHITE);
        drawEntries();
        drawFooter();
        PartialUpdate(_contentRect->x, _contentRect->y, _contentRect->w, _contentRect->h);
    }
}