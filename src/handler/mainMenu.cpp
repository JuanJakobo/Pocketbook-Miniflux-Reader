//------------------------------------------------------------------
// mainMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "mainMenu.h"

#include <string>

using std::string;

MainMenu::MainMenu(const string &name)
{
    _panelMenuHeight = ScreenHeight() / 18;
    _mainMenuWidth = ScreenWidth() / 3;
    _panelMenuBeginY = 0;
    _panelMenuBeginX = ScreenWidth() - _mainMenuWidth;

    _menuButtonRect = iRect(_mainMenuWidth * 2, _panelMenuBeginY, _mainMenuWidth, _panelMenuHeight, ALIGN_RIGHT);

    _menuFont = OpenFont("LiberationMono-Bold", _panelMenuHeight / 2, FONT_STD);

    SetFont(_menuFont, BLACK);
    DrawTextRect(0, _panelMenuBeginY, ScreenWidth(), _panelMenuHeight, name.c_str(), ALIGN_CENTER);
    DrawTextRect2(&_menuButtonRect, "Menu");
    DrawLine(0, _panelMenuHeight - 1, ScreenWidth(), _panelMenuHeight - 1, BLACK);

    _contentRect = iRect(0, _panelMenuHeight, ScreenWidth(), (ScreenHeight() - _panelMenuHeight), 0);
    
    SetPanelType(0);
    PartialUpdate(0, _panelMenuBeginY, ScreenWidth(), _panelMenuHeight);
}

MainMenu::~MainMenu()
{  
    CloseFont(_menuFont);
    free(_menu);
    free(_info);
    free(_syncDownloaded);
    free(_showDownloaded);
    free(_showUnread);
    free(_showStarred);
    free(_minifluxOverview);
    free(_markAsReadTillPage);
    free(_exit);
}

int MainMenu::createMenu(bool mainView, const iv_menuhandler &handler)
{
    imenu mainMenu[] =
        {
            {ITEM_HEADER, 0, _menu, NULL},
            {mainView ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 101, _showDownloaded, NULL},
            {mainView ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 102, _showUnread, NULL},
            {mainView ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 103, _showStarred, NULL},
            {mainView ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 104, _syncDownloaded, NULL},
            {mainView ? (short)ITEM_ACTIVE : (short)ITEM_HIDDEN, 105, _markAsReadTillPage, NULL},
            {mainView ? (short)ITEM_HIDDEN : (short)ITEM_ACTIVE, 106, _minifluxOverview, NULL},
            {ITEM_ACTIVE, 107, _info, NULL},
            {ITEM_ACTIVE, 108, _exit, NULL},
            {0, 0, NULL, NULL}};

    OpenMenu(mainMenu, 0, _panelMenuBeginX, _panelMenuBeginY, handler);

    return 1;
}
