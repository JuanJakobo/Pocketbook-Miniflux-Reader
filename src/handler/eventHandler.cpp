//------------------------------------------------------------------
// eventHandler.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "inkview.h"
#include "eventHandler.h"
#include "mainMenu.h"
#include "contextMenu.h"
#include "listView.h"
#include "util.h"
#include "log.h"

#include <string>
#include <memory>
#include <iostream>
#include <fstream>

using std::string;
using std::vector;

std::unique_ptr<EventHandler> EventHandler::_eventHandlerStatic;

EventHandler::EventHandler()
{
    //create an copy of the eventhandler to handle methods that require static functions
    _eventHandlerStatic = std::unique_ptr<EventHandler>(this);

    _listView = nullptr;

    if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
    {

        try
        {
            auto _miniflux = Miniflux(Util::readFromConfig("url"), Util::readFromConfig("token"));
            Util::connectToNetwork();
            vector<entry> entries = _miniflux.getEntries(Util::readFromConfig("filter"));
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
    case 101:
    {
        Message(ICON_INFORMATION, "Info", "Info", 1200);
        break;
    }
    //Exit
    case 102:
        CloseApp();
        break;
    default:
        break;
    }
}

void EventHandler::contextMenuHandlerStatic(const int index)
{
    _eventHandlerStatic->contextMenuHandler(index);
}
std::unique_ptr<ContextMenu> _contextMenu;

void EventHandler::contextMenuHandler(const int index)
{
    //invert color
    switch (index)
    {
    //Star
    case 101:
    {
        break;
    }
    //Comment
    case 102:
    {
        break;
    }
    //Browser
    case 103:
    {
        break;
    }
    default:
    {
        _listView->invertEntryColor(_tempItemID);
        break;
    }

        _contextMenu.reset();
    }
}

int EventHandler::pointerHandler(const int type, const int par1, const int par2)
{
    if (type == EVT_POINTERLONG)
    {
        if (_listView != nullptr)
        {
            _tempItemID = _listView->listClicked(par1, par2);
            _listView->invertEntryColor(_tempItemID);
            if (_tempItemID != -1)
            {
                _contextMenu = std::unique_ptr<ContextMenu>(new ContextMenu());
                _contextMenu->createMenu(par2, EventHandler::contextMenuHandlerStatic);
            }
        }
    }
    else if (type == EVT_POINTERUP)
    {
        //menu is clicked
        if (IsInRect(par1, par2, _menu.getMenuButtonRect()) == 1)
        {
            return _menu.createMenu(EventHandler::mainMenuHandlerStatic);
        }
        else if (_listView != nullptr)
        {
            _tempItemID = _listView->listClicked(par1, par2);
            _listView->invertEntryColor(_tempItemID);

            if (_tempItemID != -1)
            {
                //TODO change
                Log::writeLog(std::to_string(_listView->getEntry(_tempItemID)->content.length()));
                if (_listView->getEntry(_tempItemID)->content.length() < 12)
                {
                    Message(1, "test", "content empty", 1000);
                    //TODO use browser
                    //string cmd = "exec /ebrmain/bin/webbrowser.sh www.google.de";
                    //string cmd = "/ebrmain/bin/browser.app \"https://www.google.de\"";
                    //system(cmd.c_str());
                }
                else
                {
                    //TODO change path
                    string path = "/mnt/ext1/system/config/miniflux/" + std::to_string(_listView->getEntry(_tempItemID)->id) + ".html";

                    //TODO download images and set their path to local

                    std::ofstream htmlfile;
                    htmlfile.open(path);
                    htmlfile << _listView->getEntry(_tempItemID)->content;
                    htmlfile.close();

                    OpenBook(path.c_str(), "", 0);
                }

            return 1;
        }
    }
    return 0;
}

int EventHandler::keyHandler(const int type, const int par1, const int par2)
{
    //menu button
    if (type == EVT_KEYPRESS)
    {

        if (_listView != nullptr)
        {
            if (par1 == 23)
            {
                _listView->firstPage();
                return 1;
            }
            //left button -> pre page
            else if (par1 == 24)
            {
                _listView->prevPage();
                return 1;
            }
            //right button -> next page
            else if (par1 == 25)
            {
                _listView->nextPage();
                return 1;
            }
        }
    }

    return 0;
}