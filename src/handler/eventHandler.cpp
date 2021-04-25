//------------------------------------------------------------------
// eventHandler.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "eventHandler.h"

std::unique_ptr<EventHandler> EventHandler::_eventHandlerStatic;

EventHandler::EventHandler()
{
    //create an copy of the eventhandler to handle methods that require static functions
    _eventHandlerStatic = std::unique_ptr<EventHandler>(this);

    _listView = nullptr;

    if (iv_access(MINIFLUX_CONFIG_PATH.c_str(), W_OK) == 0)
    {
        _menu.drawLoadingScreen();
        if (_miniflux.login())
        {
            _listView = std::unique_ptr<ListView>(new ListView(_menu.getContentRect(), _miniflux.getItems()));
            FullUpdate();
            return;
        }
        else
        {
            Message(ICON_ERROR, "Error", "Could not login, please try again.", 1200);
        }
    }

    FullUpdate();
}

int EventHandler::eventDistributor(const int type, const int par1, const int par2)
{
    if (ISPOINTEREVENT(type))
        return EventHandler::pointerHandler(type, par1, par2);

    return 0;
}

void EventHandler::mainMenuHandlerStatic(const int index)
{
    _eventHandlerStatic->mainMenuHandler(index);
}

void EventHandler::mainMenuHandler(const int index)
{
    switch (index)
    {
    //Info
    case 104:
    {
        Message(ICON_INFORMATION, "Info", "Info", 1200);
        break;
    }
    //Exit
    case 105:
        CloseApp();
        break;
    default:
        break;
    }
}

int EventHandler::pointerHandler(const int type, const int par1, const int par2)
{
    if (type == EVT_POINTERDOWN)
    {
        //menu is clicked
        if (IsInRect(par1, par2, _menu.getMenuButtonRect()) == 1)
        {
            return _menu.createMenu(_miniflux.isLoggedIn(), _miniflux.isWorkOffline(), EventHandler::mainMenuHandlerStatic);
        }
        //if listView is shown
        else if (_listView != nullptr)
        {
            int itemID = _listView->listClicked(par1, par2);
            if (itemID != -1)
            {
                Message(ICON_INFORMATION, "Warning", "Item clicked", 1200);

            }
            PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);

            return 1;
        }
    }
    return 0;
}