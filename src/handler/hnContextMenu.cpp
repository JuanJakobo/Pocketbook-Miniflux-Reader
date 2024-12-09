//------------------------------------------------------------------
// HnContextMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "hnContextMenu.h"
#include "inkview.h"

HnContextMenu::HnContextMenu()
{
}

HnContextMenu::~HnContextMenu()
{
    free(_menu);
    free(_save);
    free(_urls);
    free(_author);
}

int HnContextMenu::createMenu(int y, const iv_menuhandler &handler)
{
    imenu contextMenu[] = {{ITEM_HEADER, 0, _menu, NULL},
                           {ITEM_ACTIVE, 101, _save, NULL},
                           {ITEM_ACTIVE, 102, _urls, NULL},
                           {ITEM_ACTIVE, 103, _author, NULL},

                           {0, 0, NULL, NULL}};

    OpenMenu(contextMenu, 0, ScreenWidth(), y, handler);

    return 0;
}
