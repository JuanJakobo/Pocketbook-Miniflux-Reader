//------------------------------------------------------------------
// hnCommentViewEntry.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "hnCommentViewEntry.h"
#include "util.h"
#include "item.h"

HnCommentViewEntry::HnCommentViewEntry(int page, irect rect, hnItem *entry) : _page(page), _position(rect), _entry(entry)
{
}

void HnCommentViewEntry::draw(ifont *entryFont, ifont *entryFontBold, int fontHeight)
{

    SetFont(entryFontBold, BLACK);
    DrawTextRect(_position.x, _position.y, _position.w, fontHeight, _entry->by.c_str(), ALIGN_LEFT);
    DrawTextRect(_position.x, _position.y, _position.w, fontHeight, std::to_string(_entry->kids.size()).c_str(), ALIGN_RIGHT);

    SetFont(entryFont, BLACK);
    
    //TODO Hn ask has both...
    if(!_entry->title.empty())
        DrawTextRect(_position.x, _position.y + 2 * fontHeight, _position.w, TextRectHeight(_position.w, _entry->title.c_str(), 0), _entry->title.c_str(), ALIGN_LEFT);

    if(!_entry->text.empty())
        DrawTextRect(_position.x, _position.y + 2 * fontHeight, _position.w, TextRectHeight(_position.w, _entry->text.c_str(), 0), _entry->text.c_str(), ALIGN_LEFT);

    //DrawTextRect(_position.x, _position.y + 3 * fontHeight, _position.w, fontHeight, test , ALIGN_RIGHT);


    int line = (_position.y + _position.h) - 1;
    DrawLine(0, line, ScreenWidth(), line, BLACK);
}