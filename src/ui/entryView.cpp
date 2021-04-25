//------------------------------------------------------------------
// entryView.cpp
//
// Author:           JuanJakobo
// Date:             25.04.2021
//
//-------------------------------------------------------------------

#include "entryView.h"

EntryView::EntryView(const entry *entry, const irect *contentRect) : _entry(entry), _contentRect(contentRect) 
{
}

void EntryView::draw(ifont *entryFont, ifont *entryFontBold, int fontHeight)
{
    SetFont(entryFontBold, BLACK);
    DrawTextRect(_contentRect->x, _contentRect->y, _contentRect->w, fontHeight, _entry->title.c_str(), ALIGN_LEFT);
    SetFont(entryFont, BLACK);

    DrawTextRect(_contentRect->x, _contentRect->y + fontHeight, _contentRect->w, _contentRect->h, _entry->content.c_str(), ALIGN_LEFT);
}