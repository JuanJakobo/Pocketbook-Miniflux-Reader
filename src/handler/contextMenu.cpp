//------------------------------------------------------------------
// contextMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "contextMenu.h"

#include <string>

ContextMenu::ContextMenu()
{
}

ContextMenu::~ContextMenu()
{
    free(_menu);
    free(_star);
    free(_download);
    free(_article);
}

int ContextMenu::createMenu(int y, const iv_menuhandler &handler, bool article, bool starred, const std::string &downloaded)
{
    std::string text = "Star";
    if(starred)
        text = "Unstar";

    _star = strdup(text.c_str());

    _download = strdup(downloaded.c_str());

    imenu contextMenu[] =
        {
            {ITEM_HEADER, 0, _menu, NULL},
            {article ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 101, _article, NULL},
            {ITEM_ACTIVE, 102, _download, NULL},
            {ITEM_ACTIVE, 103, _star, NULL},

            {0, 0, NULL, NULL}};

    OpenMenu(contextMenu, 0, ScreenWidth(), y, handler);

    return 0;
}
