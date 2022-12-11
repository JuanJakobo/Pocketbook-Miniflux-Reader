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
#include "inkview.h"

#include <string>
#include <vector>

HnCommentView::HnCommentView(const irect &contentRect, const std::vector<HnEntry> &hnEntries, int page) : ListView(contentRect, page)
{
    auto beginEntry = 0;
    auto contentHeight = _contentRect.h - _footerHeight;
    auto entrycount = hnEntries.size();

    if(entrycount > 50)
        ShowHourglassForce();

    auto i = 0;
    auto entrySize = 0;
    auto titleSize = 0;
    while (i < entrycount)
    {
        auto drawHeader = true;
        auto headerDifference = 3.5;
        auto tempHnEntry = hnEntries.at(i);

        if (!tempHnEntry.title.empty())
            titleSize = TextRectHeight(contentRect.w, tempHnEntry.title.c_str(), 0);
        else
            titleSize = 0;

        if (!tempHnEntry.text.empty())
            entrySize = TextRectHeight(contentRect.w, tempHnEntry.text.c_str(), 0);
        else
            entrySize = 0;

        entrySize = titleSize + entrySize + 2.5 * _entryFontHeight;

        while ((beginEntry + entrySize) > contentHeight)
        {
            if((beginEntry + 5 * _entryFontHeight) < contentHeight)
            {

                HnEntry tempEntry = tempHnEntry;
                tempHnEntry.text.clear();
                int tempEntrySize = titleSize + TextRectHeight(contentRect.w, tempEntry.text.c_str(), 0);

                if(!drawHeader)
                    headerDifference = 1;

                while((beginEntry + tempEntrySize + headerDifference * _entryFontHeight) > contentHeight)
                {
                    auto space = tempEntry.text.find_last_of(' ');
                    auto enter = tempEntry.text.find_last_of('\n');

                    if(enter != std::string::npos && enter > space)
                        space = enter;

                    if(space != std::string::npos){
                        tempHnEntry.text =  tempEntry.text.substr(space) + tempHnEntry.text;
                        tempEntry.text = tempEntry.text.substr(0,space);
                    }
                    else
                        break;

                    tempEntrySize = titleSize + TextRectHeight(contentRect.w, tempEntry.text.c_str(), 0);
                }

                if(!tempEntry.text.empty()){
                    if(isspace(tempEntry.text[0]))
                        tempEntry.text.erase(0,1);
                    tempEntrySize = tempEntrySize + headerDifference * _entryFontHeight;
                    tempEntry.text = tempEntry.text + "\n-->";
                    irect rect = iRect(_contentRect.x, _contentRect.y + beginEntry, _contentRect.w, tempEntrySize, 0);
                    _entries.emplace_back(std::make_unique<HnCommentViewEntry>(HnCommentViewEntry(_page, rect, tempEntry, drawHeader)));
                    if(isspace(tempHnEntry.text[0]))
                        tempHnEntry.text.erase(0,1);
                    entrySize = TextRectHeight(contentRect.w, tempHnEntry.text.c_str(), 0);
                    entrySize = entrySize + 0.5 * _entryFontHeight;

                    drawHeader = false;
                }
            }

            beginEntry = 0;
            _page++;
        }

        if(!tempHnEntry.text.empty() || !tempHnEntry.title.empty() || tempHnEntry.flagged || tempHnEntry.deleted){
            irect rect = iRect(_contentRect.x, _contentRect.y + beginEntry, _contentRect.w, entrySize, 0);
            _entries.emplace_back(std::make_unique<HnCommentViewEntry>(HnCommentViewEntry(_page, rect, tempHnEntry, drawHeader)));
            beginEntry = beginEntry + entrySize;
        }

        i++;
    }

    draw();
}
