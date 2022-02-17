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

HnCommentView::HnCommentView(const irect *contentRect, std::vector<HnEntry> *hnEntries, int page) : ListView(contentRect, page)
{
    auto beginEntry = 0;
    auto contentHeight = _contentRect->h - _footerHeight;
    auto entrycount = hnEntries->size();

    if(entrycount > 50)
        ShowHourglassForce();

    auto i = 0;
    auto entrySize = 0;
    auto titleSize = 0;
    while (i < entrycount)
    {
        auto drawHeader = true;
        auto headerDifference = 3.5;

        if (!hnEntries->at(i).title.empty())
            titleSize = TextRectHeight(contentRect->w, hnEntries->at(i).title.c_str(), 0);
        else
            titleSize = 0;

        if (!hnEntries->at(i).text.empty())
            entrySize = TextRectHeight(contentRect->w, hnEntries->at(i).text.c_str(), 0);
        else 
            entrySize = 0;

        entrySize = titleSize + entrySize + 2.5 * _entryFontHeight;

        while ((beginEntry + entrySize) > contentHeight)
        {
            //TODO why 5?
            if((beginEntry + 5 * _entryFontHeight) < contentHeight)
            {

                HnEntry tempEntry = hnEntries->at(i);
                hnEntries->at(i).text.clear();
                int tempEntrySize = titleSize + TextRectHeight(contentRect->w, tempEntry.text.c_str(), 0);

                if(!drawHeader)
                    headerDifference = 1;

                while((beginEntry + tempEntrySize + headerDifference * _entryFontHeight) > contentHeight)
                {
                    auto space = tempEntry.text.find_last_of(' ');
                    auto enter = tempEntry.text.find_last_of('\n');

                    if(enter != std::string::npos && enter > space)
                        space = enter;

                    if(space != std::string::npos){
                        hnEntries->at(i).text =  tempEntry.text.substr(space) + hnEntries->at(i).text;
                        tempEntry.text = tempEntry.text.substr(0,space);
                    }
                    else
                        break;
                    //TODO without break

                    tempEntrySize = titleSize + TextRectHeight(contentRect->w, tempEntry.text.c_str(), 0);
                }

                if(!tempEntry.text.empty()){
                    if(isspace(tempEntry.text[0]))
                        tempEntry.text.erase(0,1);
                    tempEntrySize = tempEntrySize + headerDifference * _entryFontHeight;
                    tempEntry.text = tempEntry.text + "\n-->";
                    irect rect = iRect(_contentRect->x, _contentRect->y + beginEntry, _contentRect->w, tempEntrySize, 0);
                    _entries.emplace_back(std::unique_ptr<HnCommentViewEntry>(new HnCommentViewEntry(_page, rect, tempEntry, drawHeader)));
                    if(isspace(hnEntries->at(i).text[0]))
                        hnEntries->at(i).text.erase(0,1);
                    entrySize = TextRectHeight(contentRect->w, hnEntries->at(i).text.c_str(), 0);
                    entrySize = entrySize + 0.5 * _entryFontHeight;

                    drawHeader = false;
                }
            }

            beginEntry = 0;
            _page++;
        }

        //TODO change
        if(!hnEntries->at(i).text.empty() || !hnEntries->at(i).title.empty() || hnEntries->at(i).flagged){
            irect rect = iRect(_contentRect->x, _contentRect->y + beginEntry, _contentRect->w, entrySize, 0);
            _entries.emplace_back(std::unique_ptr<HnCommentViewEntry>(new HnCommentViewEntry(_page, rect, hnEntries->at(i), drawHeader)));
            beginEntry = beginEntry + entrySize;
        }

        i++;
    }

    draw();
}
