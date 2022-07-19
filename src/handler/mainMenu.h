//------------------------------------------------------------------
// mainMenu.h
//
// Author:           JuanJakobo
// Date:             14.06.2020
// Description:      Handles the menubar and the menu
//-------------------------------------------------------------------

#ifndef MAIN_MENU
#define MAIN_MENU

#include "inkview.h"

#include <string>

class MainMenu
{
public:
    /**
        * Defines fonds, sets global Event Handler and starts new content
        *
        * @param name name of the application
        */
    MainMenu(const std::string &name);

    ~MainMenu();

    irect *getContentRect() { return &_contentRect; };
    irect *getMenuButtonRect() { return &_menuButtonRect; };

    /**
        * Shows the menu on the screen, lets the user choose menu options and then redirects the handler to the caller
        *
        * @param mainView if true mainView will be shown
        * @return int returns if the event was handled
        */
    int createMenu(bool mainView, const iv_menuhandler &handler);

private:
    ifont *_menuFont;

    int _panelMenuBeginX;
    int _panelMenuBeginY;
    int _panelMenuHeight;
    int _mainMenuWidth;
    irect _menuButtonRect;

    imenu _mainMenu;
    irect _contentRect;

    char *_menu = strdup("Menu");
    char *_syncDownloaded = strdup("Sync marked");
    char *_showUnread = strdup("Show unread");
    char *_showStarred = strdup("Show starred");
    char *_showDownloaded = strdup("Show downloaded");
    char *_markAsReadTillPage = strdup("Mark as read till page");
    char *_minifluxOverview = strdup("Go back to overview");
    char *_info = strdup("Info");
    char *_exit = strdup("Close App");

};
#endif
