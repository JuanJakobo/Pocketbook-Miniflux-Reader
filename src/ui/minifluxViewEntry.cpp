//------------------------------------------------------------------
// listViewEntry.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "minifluxViewEntry.h"

#include <memory>

MinifluxViewEntry::MinifluxViewEntry(int page, const irect &rect, const entry &item) : ListViewEntry(page, rect), _entry(std::make_unique<entry>(item))
{
}

void MinifluxViewEntry::draw(const ifont *entryFont, const ifont *entryFontBold, int fontHeight)
{
    SetFont(entryFontBold, BLACK);
    //TODO feed icon infront of text
    int heightOfTitle = TextRectHeight(_position.w, _entry->title.c_str(), 0);
    DrawTextRect(_position.x, _position.y, _position.w, heightOfTitle, _entry->title.c_str(), ALIGN_LEFT);

    SetFont(entryFont, BLACK);
    if (_entry->starred)
        DrawTextRect(_position.x, _position.y + heightOfTitle, _position.w, fontHeight, "starred", ALIGN_LEFT);
    DrawTextRect(_position.x, _position.y + heightOfTitle, _position.w, fontHeight, _entry->status.c_str(), ALIGN_RIGHT);
    if(_entry->reading_time > 0)
        DrawTextRect(_position.x, _position.y + heightOfTitle + fontHeight, _position.w, fontHeight, (std::to_string(_entry->reading_time) + " min").c_str(), ALIGN_RIGHT);

    int line = (_position.y + _position.h) - 1;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
}