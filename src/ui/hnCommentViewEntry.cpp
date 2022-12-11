//------------------------------------------------------------------
// hnCommentViewEntry.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "hnCommentViewEntry.h"
#include "inkview.h"
#include "hackernewsModel.h"

#include <ctime>
#include <chrono>
#include <iostream>
#include <time.h>

HnCommentViewEntry::HnCommentViewEntry(int page, const irect rect, const HnEntry &entry, bool drawHeader) : ListViewEntry(page, rect), _entry(entry), _drawHeader(drawHeader)
{
}

void HnCommentViewEntry::draw(const ifont *entryFont, const ifont *entryFontBold, int fontHeight) const
{
    auto beginY = _position.y;
    //header
    if(_drawHeader){
        SetFont(entryFontBold, BLACK);

        std::string header = _entry.by;
        if(_entry.time > 0)
        {
            time_t unix_timestamp = _entry.time;
            char time_buf[80];
            struct tm ts;
            ts = *localtime(&unix_timestamp);

            time_t now;
            time(&now);
            struct tm *currentTime = localtime(&now);

            auto seconds = difftime(now,mktime(&ts));

            std::string time;
            //minutes
            if(seconds > 60){
                seconds = seconds/60;
                int sec = seconds;
                time = std::to_string(sec) + " minutes ago";
                //hours
                if(seconds > 60){
                    seconds = seconds/60;
                    int sec = seconds;
                    time = std::to_string(sec) + " hours ago";
                    //days
                    if(seconds > 24){
                        seconds = seconds/24;
                        int sec = seconds;
                        time = std::to_string(sec) + " days ago";
                        //month
                        if(seconds > 30){
                            seconds = seconds/30;
                            int sec = seconds;
                            time = std::to_string(sec) + " months ago";
                            //years
                            if(seconds > 12){
                                seconds = seconds/12;
                                int sec = seconds;
                                time = std::to_string(sec) + " years ago";
                                if(seconds > 1){
                                    strftime(time_buf, sizeof(time_buf), "%y-%m-%d %h:%m:%s %z", &ts);
                                    time = time_buf;
                                }
                            }
                        }
                    }
                }
            }
            header = header + " " + time;
        }


        if (_entry.flagged)
            header = header + " [flagged]";
        else if (_entry.deleted)
            header = header + " [deleted]";

        DrawTextRect(_position.x, _position.y, _position.w, fontHeight, header.c_str(), ALIGN_LEFT);

        if (_entry.score > 0)
            DrawTextRect(_position.x, _position.y + fontHeight, _position.w, fontHeight, (std::to_string(_entry.score) + " points").c_str(), ALIGN_LEFT);

        int titleHeight = 0;
        if (!_entry.title.empty())
        {
            DrawTextRect(_position.x, _position.y, _position.w, fontHeight, (std::to_string(_entry.descendants) + " Comments").c_str(), ALIGN_RIGHT);
            titleHeight = TextRectHeight(_position.w, _entry.title.c_str(), 0);
            SetFont(entryFont, BLACK);
            DrawTextRect(_position.x, _position.y + 2 * fontHeight, _position.w, titleHeight, _entry.title.c_str(), ALIGN_LEFT);
        }
        else
        {
            std::string children;
            if (_entry.kids.size() == 0)
            {
                children = "no Childen";
            }
            else
            {
                children = std::to_string(_entry.kids.size()) + " Children";
            }

            DrawTextRect(_position.x, _position.y, _position.w, fontHeight, children.c_str(), ALIGN_RIGHT);
        }
        beginY = beginY + 1.5 * fontHeight + titleHeight;

    }

    //footer

    SetFont(entryFont, BLACK);

    if (!_entry.text.empty())
        DrawTextRect(_position.x, beginY, _position.w, _position.h, _entry.text.c_str(), ALIGN_LEFT);

    int line = (_position.y + _position.h) - 1;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
}
