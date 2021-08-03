//------------------------------------------------------------------
// hnCommentView.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "hnCommentView.h"
#include "hnCommentViewEntry.h"
#include "hackernewsModel.h"

#include <string>
#include <vector>

HnCommentView::HnCommentView(const irect *contentRect, const std::vector<HnEntry> &hnEntries, int page) : ListView(contentRect, page)
{
    auto pageHeight = 0;
    auto contentHeight = _contentRect->h - _footerHeight;
    auto entrycount = hnEntries.size();

    _entries.reserve(entrycount);

    auto i = 0;
    while (i < entrycount)
    {
        auto entrySize = 0;

        if (!hnEntries.at(i).title.empty())
        {
            entrySize = entrySize + TextRectHeight(contentRect->w, hnEntries.at(i).title.c_str(), 0);
        }

        if (!hnEntries.at(i).text.empty())
        {
            entrySize = entrySize + TextRectHeight(contentRect->w, hnEntries.at(i).text.c_str(), 0);
        }

        entrySize = entrySize + 2.5 * _entryFontHeight;

        //TODO if content is to long for one page, cut --> also do with existing... how to handle clicks on button?
        //on page x and page y both is the same article, therefore _entries can be on two pages

        if ((pageHeight + entrySize) > contentHeight)
        {
            pageHeight = 0;
            _page++;
        }
        irect rect = iRect(_contentRect->x, _contentRect->y + pageHeight, _contentRect->w, entrySize, 0);

        _entries.emplace_back(std::unique_ptr<HnCommentViewEntry>(new HnCommentViewEntry(_page, rect, hnEntries.at(i))));

        i++;
        pageHeight = pageHeight + entrySize;
    }

    draw();
}