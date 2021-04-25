//------------------------------------------------------------------
// listView.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "listView.h"

ListView::ListView(const irect *contentRect, const vector<entry> readerentries) : _contentRect(contentRect), _readerentries(readerentries)
{
    _entries.clear();

    int entrySize = _contentRect->h / (ITEMS_PER_PAGE + 1);

    _headerHeight = 0.25 * entrySize;
    _footerHeight = 0.75 * entrySize;

    _headerFontHeight = 0.8 * _headerHeight;
    _footerFontHeight = 0.3 * _footerHeight;
    _entryFontHeight = 0.2 * entrySize;

    _headerFont = OpenFont("LiberationMono", _headerFontHeight, 1);
    _footerFont = OpenFont("LiberationMono", _footerFontHeight, 1);
    _entryFont = OpenFont("LiberationMono", _entryFontHeight, 1);
    _entryFontBold = OpenFont("LiberationMono-Bold", _entryFontHeight, 1);

    _page = 1;
    _shownPage = _page;

    auto entrycount = _readerentries.size();
    auto z = 0;
    auto i = 0;
    _entries.reserve(entrycount);

    while (i < entrycount)
    {
        if (z >= ITEMS_PER_PAGE)
        {
            _page++;
            z = 0;
        }

        irect rect = iRect(_contentRect->x, z * entrySize + _headerHeight + _contentRect->y, _contentRect->w, entrySize, 0);
        this->_entries.emplace_back(_page, rect, _readerentries[i]);
        i++;
        z++;
    }

    _pageIcon = iRect(_contentRect->w - 100, _contentRect->h + _contentRect->y - _footerHeight, 100, _footerHeight, ALIGN_CENTER);

    _firstPageButton = iRect(_contentRect->x, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);
    _prevPageButton = iRect(_contentRect->x + 150, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);
    _nextPageButton = iRect(_contentRect->x + 300, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);
    _lastPageButton = iRect(_contentRect->x + 450, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);

    draw();
}

ListView::~ListView()
{
    CloseFont(_entryFont);
    CloseFont(_entryFontBold);
    CloseFont(_headerFont);
    CloseFont(_footerFont);
}

void ListView::draw()
{
    FillAreaRect(_contentRect, WHITE);
    drawHeader("Filter");
    drawEntries();
    drawFooter();
}

void ListView::drawHeader(string headerText)
{
    SetFont(_headerFont, BLACK);
    DrawTextRect(_contentRect->x, _contentRect->y, _contentRect->w, _headerHeight - 1, headerText.c_str(), ALIGN_LEFT);
    int line = (_contentRect->y + _headerHeight) - 2;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
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

void ListView::drawEntry(int itemID)
{
    FillAreaRect(_entries[itemID].getPosition(), WHITE);
    _entries[itemID].draw(_entryFont, _entryFontBold, _entryFontHeight);
}

void ListView::drawEntries()
{
    for (unsigned int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i].getPage() == _shownPage)
            _entries[i].draw(_entryFont, _entryFontBold, _entryFontHeight);
    }
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
        FillArea(_contentRect->x, _contentRect->y + _headerHeight, _contentRect->w, _contentRect->h, WHITE);
        drawEntries();
        drawFooter();
    }
}

int ListView::listClicked(int x, int y)
{
    if (IsInRect(x, y, &_firstPageButton))
    {
        actualizePage(1);
    }
    else if (IsInRect(x, y, &_nextPageButton))
    {
        actualizePage(_shownPage + 1);
    }
    else if (IsInRect(x, y, &_prevPageButton))
    {
        actualizePage(_shownPage - 1);
    }
    else if (IsInRect(x, y, &_lastPageButton))
    {
        actualizePage(_page);
    }
    else
    {
        for (unsigned int i = 0; i < _entries.size(); i++)
        {
            if (_entries[i].getPage() == _shownPage && IsInRect(x, y, _entries[i].getPosition()) == 1)
            {
                //move to central
                EntryView e = EntryView(_entries[i].getEntry(),_contentRect);
                FillAreaRect(_contentRect, WHITE);
                e.draw(_entryFont, _entryFontBold, _entryFontHeight);
                return i;
            }
        }
    }
    return -1;
}