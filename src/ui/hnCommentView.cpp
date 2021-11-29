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

HnCommentView::HnCommentView(const irect *contentRect, std::vector<HnEntry> *hnEntries, int page) : ListView(contentRect, page)
{
    auto beginEntry = 0;
    auto contentHeight = _contentRect->h - _footerHeight;
    auto entrycount = hnEntries->size();

    auto i = 0;
    auto entrySize = 0;
    auto titleSize = 0;
    while (i < entrycount)
    {
        auto drawHeader = true;

        if (!hnEntries->at(i).title.empty())
            titleSize = TextRectHeight(contentRect->w, hnEntries->at(i).title.c_str(), 0);
        else
            titleSize = 0;

        if (!hnEntries->at(i).text.empty())
            entrySize = TextRectHeight(contentRect->w, hnEntries->at(i).text.c_str(), 0);
        else 
            entrySize = 0;

        entrySize = titleSize + entrySize + 2.5 * _entryFontHeight;

        if ((beginEntry + entrySize) > contentHeight)
        {
            HnEntry tempEntry = hnEntries->at(i);
            hnEntries->at(i).text.clear();
            int tempEntrySize = titleSize + TextRectHeight(contentRect->w, tempEntry.text.c_str(), 0);

            while((beginEntry + tempEntrySize + 3.5 * _entryFontHeight) > contentHeight)
            {
                //TODO dont cut chars, cut words --> cut by spaces
                if(tempEntry.text.empty())
                    break;


                hnEntries->at(i).text = tempEntry.text.at(tempEntry.text.length()-1) + hnEntries->at(i).text;
                tempEntry.text.pop_back();
                tempEntrySize = titleSize + TextRectHeight(contentRect->w, tempEntry.text.c_str(), 0);
            }
            
            if(!tempEntry.text.empty()){
                tempEntrySize = tempEntrySize + 3.5 * _entryFontHeight;
                tempEntry.text = tempEntry.text + " -->";
                irect rect = iRect(_contentRect->x, _contentRect->y + beginEntry, _contentRect->w, tempEntrySize, 0);
                _entries.emplace_back(std::unique_ptr<HnCommentViewEntry>(new HnCommentViewEntry(_page, rect, tempEntry, true)));
                entrySize = TextRectHeight(contentRect->w, hnEntries->at(i).text.c_str(), 0);
                drawHeader = false;
                entrySize = entrySize + 0.5 * _entryFontHeight;
            }

            beginEntry = 0;
            _page++;
        }

        if(!hnEntries->at(i).text.empty() || !hnEntries->at(i).title.empty()){
            irect rect = iRect(_contentRect->x, _contentRect->y + beginEntry, _contentRect->w, entrySize, 0);
            _entries.emplace_back(std::unique_ptr<HnCommentViewEntry>(new HnCommentViewEntry(_page, rect, hnEntries->at(i), drawHeader)));
            beginEntry = beginEntry + entrySize;
        }

        i++;
    }

    draw();
}
