//------------------------------------------------------------------
// listViewEntry.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "listViewEntry.h"

ListViewEntry::ListViewEntry(int page, irect rect, entry entry) : _page(page), _position(rect), _entry(entry)
{
}

void ListViewEntry::draw(ifont *entryFont, ifont *entryFontBold, int fontHeight)
{
    SetFont(entryFontBold, BLACK);
    DrawTextRect(_position.x, _position.y, _position.w, fontHeight, _entry.title.c_str(), ALIGN_LEFT);

    SetFont(entryFont, BLACK);

    DrawTextRect(_position.x, _position.y + fontHeight, _position.w, fontHeight, _entry.url.c_str(), ALIGN_LEFT);
    //DrawTextRect(_position.x, _position.y + 2 * fontHeight, _position.w, fontHeight, item.getId(), ALIGN_LEFT);
    //DrawTextRect(_position.x, _position.y + 3 * fontHeight, _position.w, fontHeight, item.getCommentsUrl().c_str(), ALIGN_RIGHT);

    int line = (_position.y + _position.h) - 1;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
}