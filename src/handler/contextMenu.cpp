//------------------------------------------------------------------
// contextMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "contextMenu.h"

ContextMenu::ContextMenu()
{
}

ContextMenu::~ContextMenu()
{
    free(_menu);
    free(_star);
    free(_comments);
    free(_browser);
}

int ContextMenu::createMenu(int y, const iv_menuhandler &handler)
{
    imenu contextMenu[] =
        {
            {ITEM_HEADER, 0, _menu, NULL},
            {ITEM_ACTIVE, 101, _star, NULL},
            {ITEM_ACTIVE, 102, _comments, NULL},
            {ITEM_ACTIVE, 103, _browser, NULL},

            {0, 0, NULL, NULL}};

    OpenMenu(contextMenu, 0, ScreenWidth(), y, handler);

    return 1;
}