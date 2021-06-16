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
#include "hackernews.h"
#include "item.h"
#include "hnCommentView.h"

#include <string>
#include <memory>
#include <iostream>
#include <fstream>

using std::string;
using std::vector;

std::unique_ptr<EventHandler> EventHandler::_eventHandlerStatic;
pthread_mutex_t mutexEntries;

EventHandler::EventHandler()
{
    //create an copy of the eventhandler to handle methods that require static functions
    _eventHandlerStatic = std::unique_ptr<EventHandler>(this);

    _minifluxView = nullptr;

    if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
    {
        if (iv_access(ARTICLE_FOLDER.c_str(), W_OK) != 0)
            iv_mkdir(ARTICLE_FOLDER.c_str(), 0777);

        try
        {
            drawMiniflux();
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
        ShowHourglassForce();

        //TODO only show if is hn!
        try
        {
            auto parentCommentItemID = _minifluxView->getEntry(_tempItemID)->comments_url;

            auto end = parentCommentItemID.find("id=");
            parentCommentItemID = parentCommentItemID.substr(end + 3);
            _minifluxView.reset();
            drawHN(atoi(parentCommentItemID.c_str()));
        }
        catch (const std::exception &e)
        {
            Log::writeLog(e.what());
        }

        break;
    }
    //Browser
    case 103:
    {
        break;
    }
    default:
    {
        _minifluxView->invertEntryColor(_tempItemID);
        break;
    }

        _contextMenu.reset();
    }
}

int EventHandler::pointerHandler(const int type, const int par1, const int par2)
{
    if (type == EVT_POINTERLONG)
    {
        if (_minifluxView != nullptr)
        {
            _tempItemID = _minifluxView->listClicked(par1, par2);
            _minifluxView->invertEntryColor(_tempItemID);
            if (_tempItemID != -1)
            {
                bool comments = false;
                if (_minifluxView->getEntry(_tempItemID)->comments_url.find("news.ycombinator.com") != std::string::npos)
                    comments = true;

                _contextMenu = std::unique_ptr<ContextMenu>(new ContextMenu());
                _contextMenu->createMenu(par2, EventHandler::contextMenuHandlerStatic, comments);
                return 1;
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
        else if (_minifluxView != nullptr)
        {
            _tempItemID = _minifluxView->listClicked(par1, par2);

            if (_tempItemID != -1)
            {
                _minifluxView->invertEntryColor(_tempItemID);
                ShowHourglassForce();

                if (_minifluxView->getEntry(_tempItemID)->content.length() < 12)
                {
                    //TODO use browser --> in child??
                    //string cmd = "exec /ebrmain/bin/webbrowser.sh www.google.de";
                    string cmd = "/ebrmain/bin/browser.app \"https://www.google.de\"";
                    execlp(cmd.c_str(), cmd.c_str(), (char *)NULL);
                }
                else
                {
                    string path = ARTICLE_FOLDER + "/" + std::to_string(_minifluxView->getEntry(_tempItemID)->id) + ".html";
                    if (iv_access(path.c_str(), W_OK) != 0)
                    {
                        //TODO download images and set their path to local
                        std::ofstream htmlfile;
                        htmlfile.open(path);
                        htmlfile << _minifluxView->getEntry(_tempItemID)->content;
                        htmlfile.close();
                        Log::writeLog(path);
                    }

                    OpenBook(path.c_str(), "", 0);
                }

                _minifluxView->invertEntryColor(_tempItemID);
            }
            return 1;
        }
        else if (_hnCommentView != nullptr)
        {
            //todo rename
            int clickedItemIDHN = _hnCommentView->listClicked(par1, par2);

            //here does not work
            if (clickedItemIDHN != -1)
            {
                _hnCommentView->invertEntryColor(clickedItemIDHN);
                ShowHourglassForce();

                if (clickedItemIDHN == 0)
                {
                    if (_hnCommentView->getEntry(clickedItemIDHN)->parent != 0)
                    {
                        //TODO go back where one left of

                        drawHN(_hnCommentView->getEntry(clickedItemIDHN)->parent);
                    }
                    else
                    {
                        try
                        {
                            drawMiniflux();
                        }
                        catch (const std::exception &e)
                        {
                            Message(ICON_ERROR, "Error", e.what(), 1200);
                        }
                    }
                }
                else
                {
                    drawHN(_hnCommentView->getEntry(clickedItemIDHN)->id);
                }
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

        if (_minifluxView != nullptr)
        {
            if (par1 == 23)
            {
                _minifluxView->firstPage();
                return 1;
            }
            //left button -> pre page
            else if (par1 == 24)
            {
                _minifluxView->prevPage();
                return 1;
            }
            //right button -> next page
            else if (par1 == 25)
            {
                _minifluxView->nextPage();
                return 1;
            }
        }
        else if (_hnCommentView != nullptr)
        {
            //go back one page
            if (par1 == 23)
            {
                if (_hnCommentView->getEntry(0)->parent != 0)
                {
                    drawHN(_hnCommentView->getEntry(0)->parent);
                }
                else
                {
                    //TODO when is main comment go back to miniflux?
                    _hnCommentView->firstPage();
                }
                return 1;
            }
            //left button -> pre page
            else if (par1 == 24)
            {
                _hnCommentView->prevPage();
                return 1;
            }
            //right button -> next page
            else if (par1 == 25)
            {
                _hnCommentView->nextPage();
                return 1;
            }
        }
    }

    return 0;
}

void *EventHandler::itemToEntries(void *arg)
{
    //TODO make static? and just once...
    auto hn = Hackernews();
    hnItem temp = hn.getItem(*(int *)arg);

    pthread_mutex_lock(&mutexEntries);
    _eventHandlerStatic->_hnItems.push_back(temp);
    pthread_mutex_unlock(&mutexEntries);

    return NULL;
}

void EventHandler::drawMiniflux()
{
    auto _miniflux = Miniflux(Util::readFromConfig("url"), Util::readFromConfig("token"));
    Util::connectToNetwork();
    std::vector<entry> entries = _miniflux.getEntries(Util::readFromConfig("filter"));
    _minifluxView = std::unique_ptr<MinifluxView>(new MinifluxView(_menu.getContentRect(), entries));
    _hnCommentView.reset();
    _hnItems.clear();

    FullUpdate();
}

void EventHandler::drawHN(int itemID)
{
    auto hn = Hackernews();

    auto found = false;

    hnItem parentItem;
    std::vector<hnItem> currentHnComments;

    Log::writeLog(std::to_string(_hnItems.size()));

    //test if is already in Database
    for (size_t i = 0; i < _hnItems.size(); i++)
    {

        if (_hnItems.at(i).id == itemID)
        {
            parentItem = _hnItems.at(i);
            found = true;
            break;
        }
    }

    if (!found)
    {
        Util::connectToNetwork();
        parentItem = hn.getItem(itemID);
    }

    if (parentItem.kids.size() == 0)
    {
        Message(ICON_INFORMATION, "Info", "This Comment has no childs.", 1000);
    }
    else
    {

        //when item is child make text ...
        if (parentItem.parent > 0 && !parentItem.text.empty())
            parentItem.text = "...";

        _hnItems.push_back(parentItem);

        currentHnComments.push_back(_hnItems.back());

        //test if items have already been downloaded
        vector<int> tosearch;

        for (size_t i = 0; i < parentItem.kids.size(); ++i)
        {
            found = false;

            for (size_t j = 0; j < _hnItems.size(); ++j)
            {
                if (parentItem.kids.at(i) == _hnItems.at(j).id)
                {
                    currentHnComments.push_back(_hnItems.at(j));
                    found = true;
                    break;
                }
            }

            if (!found)
                tosearch.push_back(parentItem.kids.at(i));
        }

        //Download comments
        auto threadCount = tosearch.size();
        pthread_t threads[threadCount];
        mutexEntries = PTHREAD_MUTEX_INITIALIZER;
        int count;

        Util::connectToNetwork();

        for (count = 0; count < threadCount; ++count)
        {
            if (pthread_create(&threads[count], NULL, itemToEntries, &parentItem.kids.at(count)) != 0)
            {
                Log::writeLog("could not create thread");
                break;
            }
        }

        for (size_t i = 0; i < count; ++i)
        {
            if (pthread_join(threads[i], NULL) != 0)
                Log::writeLog("cannot join thread" + std::to_string(i));
        }

        pthread_mutex_destroy(&mutexEntries);

        //sort items in the correct order
        for (size_t i = 0; i < parentItem.kids.size(); ++i)
        {
            for (size_t j = 0; j < _hnItems.size(); ++j)
            {
                if (parentItem.kids.at(i) == _hnItems.at(j).id)
                {
                    currentHnComments.push_back(_hnItems.at(j));
                    break;
                }
            }
        }

        _hnCommentView.reset(new HnCommentView(_menu.getContentRect(), currentHnComments));

        PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);
    }
}
