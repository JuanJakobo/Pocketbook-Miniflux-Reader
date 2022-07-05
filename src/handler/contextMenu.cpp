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
    free(_comments);
}

int ContextMenu::createMenu(int y, const iv_menuhandler &handler, bool comments, bool starred, const std::string &downloaded)
{
    std::string text = "Star";
    if(starred)
        text = "Unstar";

    _star = strdup(text.c_str());

		_download = strdup(downloaded.c_str());

    imenu contextMenu[] =
        {
            {ITEM_HEADER, 0, _menu, NULL},
            {comments ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 101, _comments, NULL},
            {ITEM_ACTIVE, 102, _download, NULL},
            {ITEM_ACTIVE, 103, _star, NULL},

            {0, 0, NULL, NULL}};

    OpenMenu(contextMenu, 0, ScreenWidth(), y, handler);

    return 0;
}
