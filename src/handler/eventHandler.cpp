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
    if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
    {
        _menu.drawLoadingScreen();

        try
        {
            auto _miniflux = Miniflux(Util::readFromConfig("url"), Util::readFromConfig("token"));
            Util::connectToNetwork();
            vector<entry> entries = _miniflux.getEntries();
            _listView = std::unique_ptr<ListView>(new ListView(_menu.getContentRect(), entries));
            FullUpdate();
        }
        catch (const std::exception &e)
        {
            Message(ICON_ERROR, "Error", e.what(), 1200);
        }
    }
}

int EventHandler::eventDistributor(const int type, const int par1, const int par2)
{
    if (ISPOINTEREVENT(type))
        return EventHandler::pointerHandler(type, par1, par2);
    else if (ISKEYEVENT(type))
        return EventHandler::keyHandler(type, par1, par2);

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
            return _menu.createMenu(true, true, EventHandler::mainMenuHandlerStatic);
            //return _menu.createMenu(_miniflux.isLoggedIn(), _miniflux.isWorkOffline(), EventHandler::mainMenuHandlerStatic);
        }
        //else if (_entryView != nullptr)
        //{
            
        //}
        //if listView is shown
        else if (_listView != nullptr)
        {
            int itemID = _listView->listClicked(par1, par2);
            if (itemID != -1)
            {
            }
            PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);

            return 1;
        }
    }
    return 0;
}

int EventHandler::keyHandler(const int type, const int par1, const int par2)
{
    //menu button
    if (par1 == 23)
    {
        _listView->draw();
        _entryView = nullptr;
        PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);
        return 1;
    }
    if (_entryView != nullptr)
    {
        //left button -> pre page
        if (par1 == 24)
        {
            //next page with text, how to handle images?
        }
        //right button -> next page
        else if (par1 == 25)
        {
        }
    }
    else if (_listView != nullptr)
    {
        //left button -> pre page
        if (par1 == 24)
        {
        }
        //right button -> next page
        else if (par1 == 25)
        {
        }
    }

    return 0;
}