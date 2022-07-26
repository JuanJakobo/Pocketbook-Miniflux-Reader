//------------------------------------------------------------------
// MinifluxView.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "minifluxView.h"
#include "minifluxViewEntry.h"
#include "minifluxModel.h"

#include <string>
#include <vector>

using std::vector;

MinifluxView::MinifluxView(const irect &contentRect, const vector<MfEntry> &mfEntries, int page) : ListView(contentRect, page)
{
    auto pageHeight = 0;
    auto contentHeight = _contentRect.h - _footerHeight;
    auto entrycount = mfEntries.size();

    _entries.reserve(entrycount);

    auto i = 0;
    while (i < entrycount)
    {
        auto entrySize = TextRectHeight(contentRect.w*1.01, mfEntries.at(i).title.c_str(), 0) + 2.5 * _entryFontHeight;

        if ((pageHeight + entrySize) > contentHeight)
        {
            pageHeight = 0;
            _page++;
        }
        irect rect = iRect(_contentRect.x, _contentRect.y + pageHeight, _contentRect.w, entrySize, 0);

        _entries.emplace_back(std::make_unique<MinifluxViewEntry>(MinifluxViewEntry(_page, rect, mfEntries.at(i))));

        i++;
        pageHeight = pageHeight + entrySize;
    }
    draw();
}

vector<int> MinifluxView::getEntriesTillPage()
{
    std::vector<int> temp;
    {
        for (size_t i = 0; i < _entries.size(); i++)
        {
            if (_entries.at(i)->getPage() <= _shownPage)
                temp.push_back(getEntry(i).id);
        }
    }
    return temp;
}
