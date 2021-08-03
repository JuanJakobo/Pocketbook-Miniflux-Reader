//------------------------------------------------------------------
// HnContextMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "hnContextMenu.h"

HnContextMenu::HnContextMenu()
{
}

HnContextMenu::~HnContextMenu()
{
    free(_menu);
    free(_save);
    free(_author);
    free(_urls);
}

int HnContextMenu::createMenu(int y, const iv_menuhandler &handler)
{
    imenu contextMenu[] =
        {
            {ITEM_HEADER, 0, _menu, NULL},
            {ITEM_ACTIVE, 101, _save, NULL},
            {ITEM_ACTIVE, 102, _author, NULL},
            {ITEM_ACTIVE, 103, _urls, NULL},

            {0, 0, NULL, NULL}};

    OpenMenu(contextMenu, 0, ScreenWidth(), y, handler);

    return 1;
}