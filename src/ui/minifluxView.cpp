//------------------------------------------------------------------
// MinifluxView.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "model.h"
#include "minifluxView.h"
#include "minifluxViewEntry.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

MinifluxView::MinifluxView(const irect *contentRect, const vector<entry> &readerentries, int page) : ListView(contentRect, page), _readerentries(std::move(readerentries))
{
    int entrySize;

    int pageHeight = 0;

    _footerHeight = _contentRect->h / 10;

    int contentHeight = _contentRect->h - _footerHeight;

    _footerFontHeight = 0.3 * _footerHeight;
    _entryFontHeight = 30; //0.2 * _footerFontHeight;//entrySize; //TODO how much? --> add for both

    _footerFont = OpenFont("LiberationMono", _footerFontHeight, 1);
    _entryFont = OpenFont("LiberationMono", _entryFontHeight, 1);
    _entryFontBold = OpenFont("LiberationMono-Bold", _entryFontHeight, 1);

    auto entrycount = _readerentries.size();

    auto i = 0;
    _entries.reserve(entrycount);
    
    SetFont(_entryFont, BLACK);

    while (i < entrycount)
    {
        entrySize = TextRectHeight(contentRect->w, _readerentries.at(i).title.c_str(), 0) + 2.5 * _entryFontHeight;

        if ((pageHeight + entrySize) > contentHeight)
        {
            pageHeight = 0;
            _page++;
        }
        irect rect = iRect(_contentRect->x, _contentRect->y + pageHeight, _contentRect->w, entrySize, 0);

        _entries.emplace_back(std::unique_ptr<MinifluxViewEntry>(new MinifluxViewEntry(_page, rect, _readerentries.at(i))));

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