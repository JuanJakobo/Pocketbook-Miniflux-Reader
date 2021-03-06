//------------------------------------------------------------------
// HnContextMenu.h
//
// Author:           JuanJakobo
// Date:             14.06.2020
// Description:      Handles the menubar and the menu
//-------------------------------------------------------------------

#ifndef HN_CONTEXT_MENU
#define HN_CONTEXT_MENU

#include "inkview.h"

class HnContextMenu
{
public:
    HnContextMenu();

    ~HnContextMenu();

    /**
        * Shows the menu on the screen, lets the user choose menu options and then redirects the handler to the caller
        *
        * @param y y-coordinate of the item
        * @param handler  which action does the menu buttons start
        * @return int returns if the event was handled
        */
    int createMenu(int y, const iv_menuhandler &handler);

private:
    char *_menu = strdup("Menu");
    char *_save = strdup("Save as note");
    char *_author = strdup("Show Authordetails");
    char *_urls = strdup("Show Comment URLs");
};
#endif
