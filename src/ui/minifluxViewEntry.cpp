//------------------------------------------------------------------
// listViewEntry.cpp
//
// Author:           JuanJakobo
// Date:             04.08.2020
//
//-------------------------------------------------------------------

#include "minifluxViewEntry.h"
#include "minifluxModel.h"

#include <string>

MinifluxViewEntry::MinifluxViewEntry(int page, const irect &position, const MfEntry &entry) : ListViewEntry(page, position) , _entry(entry)
{
}

void MinifluxViewEntry::draw(const ifont *entryFont, const ifont *entryFontBold, int fontHeight) const
{
    SetFont(entryFontBold, BLACK);
    int heightOfTitle = TextRectHeight(_position.w, _entry.title.c_str(), 0);
    DrawTextRect(_position.x, _position.y, _position.w, heightOfTitle, _entry.title.c_str(), ALIGN_LEFT);

    SetFont(entryFont, BLACK);
    auto shortendURL = _entry.url;
    auto start = 0;
    auto counter = 0;

    while ((start = shortendURL.find("/", start)) != std::string::npos && counter < 2)
    {
        start += 1;
        counter++;
    }

    shortendURL = shortendURL.substr(0, start);

    DrawTextRect(_position.x, _position.y + heightOfTitle, _position.w, fontHeight, shortendURL.c_str(), ALIGN_LEFT);

    std::string textLeft;
    if(_entry.downloaded == IsDownloaded::TOBEDOWNLOADED)
        textLeft = "to be downloaded";
    else if(_entry.downloaded == IsDownloaded::DOWNLOADED)
        textLeft = "downloaded";

    if (_entry.starred)
    {
        if(!textLeft.empty())
            textLeft += " || ";
        textLeft += "starred";
    }

    DrawTextRect(_position.x, _position.y + heightOfTitle + fontHeight, _position.w, fontHeight, textLeft.c_str(), ALIGN_LEFT);
    DrawTextRect(_position.x, _position.y + heightOfTitle, _position.w, fontHeight, _entry.status.c_str(), ALIGN_RIGHT);
    if (_entry.reading_time > 0)
        DrawTextRect(_position.x, _position.y + heightOfTitle + fontHeight, _position.w, fontHeight, (std::to_string(_entry.reading_time) + " min").c_str(), ALIGN_RIGHT);

    drawBottomLine();
}
