//------------------------------------------------------------------
// hnCommentView.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "hnCommentView.h"
#include "hnCommentViewEntry.h"
#include "item.h"
#include "util.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;


HnCommentView::HnCommentView(const irect *contentRect, vector<hnItem> *readerentries, int page) : _contentRect(contentRect)
{

    _entries.clear();

    int entrySize;

    int pageHeight = 0;

    _footerHeight = _contentRect->h / 10;

    int contentHeight = _contentRect->h - _footerHeight;

    _footerFontHeight = 0.3 * _footerHeight;
    _entryFontHeight = 30; //0.2 * _footerFontHeight;//entrySize; //how much?

    _footerFont = OpenFont("LiberationMono", _footerFontHeight, 1);
    _entryFont = OpenFont("LiberationMono", _entryFontHeight, 1);
    _entryFontBold = OpenFont("LiberationMono-Bold", _entryFontHeight, 1);

    _page = 1;
    _shownPage = page;

    auto entrycount = readerentries->size();

    auto i = 0;
    _entries.reserve(entrycount);

    SetFont(_entryFont, BLACK);

    while (i < entrycount)
    {
        if (!readerentries->at(i).title.empty())
        {
            //uses the font that is currently set
            entrySize = TextRectHeight(contentRect->w, readerentries->at(i).title.c_str(), 0) + _entryFontHeight * 4;
        }
        else if (!readerentries->at(i).text.empty())
        {
            entrySize = TextRectHeight(contentRect->w, readerentries->at(i).text.c_str(), 0) + _entryFontHeight * 4;
        }

        //TODO if content is to long for one page, cut --> also do with existing... how to handle clicks on button?
        //on page x and page y both is the same article, therefore _entries can be on two pages 

        if ((pageHeight + entrySize) > contentHeight)
        {
            pageHeight = 0;
            _page++;
        }
        irect rect = iRect(_contentRect->x, _contentRect->y + pageHeight, _contentRect->w, entrySize, 0);

        this->_entries.emplace_back(_page, rect, &readerentries->at(i));
        i++;
        pageHeight = pageHeight + entrySize;
    }

    _pageIcon = iRect(_contentRect->w - 100, _contentRect->h + _contentRect->y - _footerHeight, 100, _footerHeight, ALIGN_CENTER);

    _firstPageButton = iRect(_contentRect->x, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);
    _prevPageButton = iRect(_contentRect->x + 150, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);
    _nextPageButton = iRect(_contentRect->x + 300, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);
    _lastPageButton = iRect(_contentRect->x + 450, _contentRect->h + _contentRect->y - _footerHeight, 130, _footerHeight, ALIGN_CENTER);

    draw();
}

HnCommentView::~HnCommentView()
{
    CloseFont(_entryFont);
    CloseFont(_entryFontBold);
    CloseFont(_footerFont);
}

void HnCommentView::draw()
{
    FillAreaRect(_contentRect, WHITE);
    drawEntries();
    drawFooter();
}

void HnCommentView::drawEntry(int itemID)
{
    FillAreaRect(_entries[itemID].getPosition(), WHITE);
    _entries[itemID].draw(_entryFont, _entryFontBold, _entryFontHeight);
    updateEntry(itemID);
}

void HnCommentView::invertEntryColor(int itemID)
{
    InvertAreaBW(_entries[itemID].getPosition()->x, _entries[itemID].getPosition()->y, _entries[itemID].getPosition()->w, _entries[itemID].getPosition()->h);
    updateEntry(itemID);
}

void HnCommentView::drawEntries()
{
    for (unsigned int i = 0; i < _entries.size(); i++)
    {
        if (_entries[i].getPage() == _shownPage)
            _entries[i].draw(_entryFont, _entryFontBold, _entryFontHeight);
    }
}

int HnCommentView::listClicked(int x, int y)
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
            if (_entries[i].getPage() == _shownPage && IsInRect(x, y, _entries[i].getPosition()) == 1)
            {
                invertEntryColor(i);
                return i;
            }
        }
    }
    return -1;
}

void HnCommentView::drawFooter()
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

void HnCommentView::updateEntry(int itemID)
{
    PartialUpdate(_entries[itemID].getPosition()->x, _entries[itemID].getPosition()->y, _entries[itemID].getPosition()->w, _entries[itemID].getPosition()->h);
}

void HnCommentView::actualizePage(int pageToShown)
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