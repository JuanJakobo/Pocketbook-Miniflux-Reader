//------------------------------------------------------------------
// contextMenu.h
//
// Author:           JuanJakobo
// Date:             14.06.2020
// Description:      Handles the menubar and the menu
//-------------------------------------------------------------------

#ifndef CONTEXT_MENU
#define CONTEXT_MENU

#include "inkview.h"

class ContextMenu
{
public:
    ContextMenu();

    ~ContextMenu();

    /**
        * Shows the menu on the screen, lets the user choose menu options and then redirects the handler to the caller
        * 
        * @param y y-coordinate of the item 
        * @param handler  which action does the menu buttons start 
        * @return int returns if the event was handled
        */
    int createMenu(int y, const iv_menuhandler &handler, bool comments);

private:
    char *_menu = strdup("Menu");
    char *_star = strdup("Star");
    char *_comments = strdup("Comments");
    char *_browser = strdup("Browser");
};
#endif