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

#include "log.h"

#include <ctime>
#include <iostream>

HnCommentViewEntry::HnCommentViewEntry(int page, const irect rect, const HnEntry &entry) : ListViewEntry(page, rect), _entry(entry)
{
}

void HnCommentViewEntry::draw(const ifont *entryFont, const ifont *entryFontBold, int fontHeight)
{

    SetFont(entryFontBold, BLACK);
    DrawTextRect(_position.x, _position.y, _position.w, fontHeight, _entry.by.c_str(), ALIGN_LEFT);
    DrawTextRect(_position.x, _position.y, _position.w, fontHeight, (std::to_string(_entry.kids.size()) + " Children").c_str(), ALIGN_RIGHT);

    if (_entry.score > 0)
        DrawTextRect(_position.x, _position.y + fontHeight, _position.w, fontHeight, ("Karma " + std::to_string(_entry.score)).c_str(), ALIGN_LEFT);
   
    DrawTextRect(_position.x, _position.y + fontHeight, _position.w, fontHeight, std::to_string(_entry.time).c_str(), ALIGN_RIGHT);

    SetFont(entryFont, BLACK);

    int titleHeight = 0;
    if (!_entry.title.empty())
    {
        titleHeight = TextRectHeight(_position.w, _entry.title.c_str(), 0);
        DrawTextRect(_position.x, _position.y + 2 * fontHeight, _position.w, titleHeight, _entry.title.c_str(), ALIGN_LEFT);
    }

    if (!_entry.text.empty())
        DrawTextRect(_position.x, _position.y + 2 * fontHeight + titleHeight, _position.w, TextRectHeight(_position.w, _entry.text.c_str(), 0), _entry.text.c_str(), ALIGN_LEFT);

    int line = (_position.y + _position.h) - 1;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
}