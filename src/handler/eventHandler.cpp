//------------------------------------------------------------------
// eventHandler.cpp
//
// Author:           JuanJakobo
// Date:             22.04.2021
//
//-------------------------------------------------------------------

#include "eventHandler.h"
#include "inkview.h"

#include "mainMenu.h"
#include "contextMenu.h"
#include "hnContextMenu.h"

#include "miniflux.h"
#include "minifluxModel.h"
#include "minifluxView.h"

#include "hackernews.h"
#include "hackernewsModel.h"
#include "hnCommentView.h"

#include "util.h"
#include "log.h"
#include "sqliteConnector.h"

#include <string>
#include <memory>
#include <map>
#include <vector>

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

        if (iv_access(IMAGE_FOLDER.c_str(), W_OK) != 0)
            iv_mkdir(IMAGE_FOLDER.c_str(), 0777);

        try
        {

            _miniflux = std::unique_ptr<Miniflux>(new Miniflux(Util::readFromConfig("url"), Util::readFromConfig("token")));
            Util::connectToNetwork();

            //TODO implement once custom filter is available
            //filter in menubar ! --> as in PB lib
            //string filter = Util::readFromConfig("filter");
            //if(filter.empty())
            string filter = "status=unread&direction=desc";
            _entries = _miniflux->getEntries(filter);
            drawMiniflux();
        }
        catch (const std::exception &e)
        {
            Message(ICON_ERROR, "Error", e.what(), 1200);
        }
    }
    else
    {
        Message(ICON_INFORMATION, "Config", ("Please create " + CONFIG_PATH + " containing username and token. Example is provided in the Repo.").c_str(), 5000);
        CloseApp();
    }
}

EventHandler::~EventHandler()
{
    //TODO stop database
    Log::writeLogInfo("delete eventHandler");
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
    //show unread
    case 101:
    {
        drawMiniflux(Util::readFromConfig("filter"));
        break;
    }
    //show starred
    case 102:
    {
        drawMiniflux("starred=true&direction=desc&limit=1000");
        break;
    }
    //download starred
    case 103:
    {
        //TODO implement download for hn, how to store articles?
        ShowHourglassForce();
        //no interent chec happens
        //get starred
        break;
    }
    //Mark as read till page
    case 104:
    {
        ShowHourglassForce();
        //TODO where to do network check?
        Util::connectToNetwork();
        if (!_miniflux->updateEntries(_minifluxView->getEntriesTillPage(), true))
            Log::writeLogError("Could not get entries");
        drawMiniflux(Util::readFromConfig("filter"));
        break;
    }
    //Go back to miniflux overview
    case 105:
    {
        drawMiniflux("", _minifluxViewShownPage);
        break;
    }
    case 106:
    {
        Message(ICON_INFORMATION, "Info", "Info", 1200);
        break;
    }
    //Exit
    case 107:
        CloseApp();
        break;
    default:
        break;
    }
}

void EventHandler::hnContextMenuHandlerStatic(const int index)
{
    _eventHandlerStatic->hnContextMenuHandler(index);
}

void EventHandler::hnContextMenuHandler(const int index)
{
    switch (index)
    {
    //Save note
    case 101:
    {
        //TODO save notes
        break;
    }
    //author
    case 102:
    {
        Util::connectToNetwork();
        ShowHourglassForce();
        HnUser user = Hackernews::getUser(_hnCommentView->getCurrentEntry()->by);
        Util::decodeHTML(user.about);
        string message = "User: " + user.id + "\n Karma: " + std::to_string(user.karma) + "\n About: " + user.about + "\n Created: " + std::to_string(user.created);
        DialogSynchro(ICON_INFORMATION, "User information", message.c_str(), "Close", NULL, NULL);
        _hnCommentView->reDrawCurrentEntry();
        break;
    }
    //urls
    case 103:
    {
        //TODO show urls off the command and make clickable,  also in dialog, can then be clicked?
        // open hackernews in comments again
    }
    default:
    {
        _hnCommentView->invertCurrentEntryColor();
        break;
    }
        _hnContextMenu.reset();
    }
}
void EventHandler::contextMenuHandlerStatic(const int index)
{
    _eventHandlerStatic->contextMenuHandler(index);
}

void EventHandler::contextMenuHandler(const int index)
{
    switch (index)
    {
    //Unstar/Star
    case 101:
    {
        if (_miniflux->toggleBookmark(_minifluxView->getCurrentEntry()->id))
        {
            _minifluxView->getCurrentEntry()->starred = !_minifluxView->getCurrentEntry()->starred;
            _minifluxView->reDrawCurrentEntry();
        }
        else
        {
            Message(ICON_ERROR, "Error", "Could not starr the selected item.", 1200);
            _minifluxView->invertCurrentEntryColor();
        }
        break;
    }
    //Comment
    case 102:
    {
        ShowHourglassForce();
        auto parentCommentItemID = _minifluxView->getCurrentEntry()->comments_url;

        auto end = parentCommentItemID.find("id=");
        parentCommentItemID = parentCommentItemID.substr(end + 3);
        _minifluxViewShownPage = _minifluxView->getShownPage();
        drawHN(atoi(parentCommentItemID.c_str()));

        break;
    }
    //Browser
    case 103:
    {
        Util::openInBrowser(_minifluxView->getCurrentEntry()->url);
        break;
    }
    default:
    {
        _minifluxView->invertCurrentEntryColor();
        break;
    }
    }
}

int EventHandler::pointerHandler(const int type, const int par1, const int par2)
{
    if (type == EVT_POINTERLONG)
    {
        if (_currentView == Views::MFVIEW)
        {
            if (_minifluxView->checkIfEntryClicked(par1, par2))
                _minifluxView->invertCurrentEntryColor();

            bool comments = false;
            if (_minifluxView->getCurrentEntry()->comments_url.find("news.ycombinator.com") != std::string::npos)
                comments = true;

            _contextMenu.createMenu(par2, EventHandler::contextMenuHandlerStatic, comments, _minifluxView->getCurrentEntry()->starred);
            return 1;
        }
        else if (_currentView == Views::HNCOMMENTSVIEW)
        {
            if (_hnCommentView->checkIfEntryClicked(par1, par2))
            {
                _hnCommentView->invertCurrentEntryColor();
                _hnContextMenu = std::unique_ptr<HnContextMenu>(new HnContextMenu());
                _hnContextMenu->createMenu(par2, EventHandler::hnContextMenuHandlerStatic);
                return 1;
            }
        }
    }
    else if (type == EVT_POINTERUP)
    {
        //if menu is clicked
        if (IsInRect(par1, par2, _menu.getMenuButtonRect()) == 1)
        {
            auto mainView = true;
            if (_hnCommentView != nullptr)
                mainView = false;

            return _menu.createMenu(mainView, EventHandler::mainMenuHandlerStatic);
        }
        else if (_currentView == Views::MFVIEW)
        {
            if (_minifluxView->checkIfEntryClicked(par1, par2))
            {
                _minifluxView->invertCurrentEntryColor();
                ShowHourglassForce();

                if (_minifluxView->getCurrentEntry()->reading_time == 0)
                {
                    Util::openInBrowser(_minifluxView->getCurrentEntry()->url);
                }
                else
                {
                    //open the comment view directly if is HN
                    if (_minifluxView->getCurrentEntry()->url.find("news.ycombinator.com") != std::string::npos)
                    {
                        auto parentCommentItemID = _minifluxView->getCurrentEntry()->comments_url;

                        auto end = parentCommentItemID.find("id=");
                        parentCommentItemID = parentCommentItemID.substr(end + 3);
                        _minifluxViewShownPage = _minifluxView->getShownPage();
                        drawHN(atoi(parentCommentItemID.c_str()));
                        return 1;
                    }

                    //remove chars that are not allowed in filenames
                    const std::string forbiddenInFiles = "<>\\/:?\"|";

                    std::transform(title.begin(), title.end(), title.begin(), [&forbiddenInFiles](char c) { return forbiddenInFiles.find(c) != std::string::npos ? ' ' : c; });

                    string path = ARTICLE_FOLDER + "/" + title + ".html";
                    if (iv_access(path.c_str(), W_OK) != 0)
                    {
                        string content = _minifluxView->getEntry(_tempItemID)->content;
                        string result = content;
                        try
                        {
                            while (content.length() > 0)
                            {
                                auto found = content.find("<img");

                                if (found != std::string::npos)
                                {
                                    auto imageFolder = "img/" + title;

                                    if (iv_access((ARTICLE_FOLDER + "/" + imageFolder).c_str(), W_OK) != 0)
                                        iv_mkdir((ARTICLE_FOLDER + "/" + imageFolder).c_str(), 0777);

                                    auto imagePath = imageFolder + "/" + std::to_string(found);

                                    if (iv_access((ARTICLE_FOLDER + "/" + imagePath).c_str(), W_OK) != 0)
                                    {
                                        content = content.substr(found);
                                        auto src = content.find("src=\"");
                                        content = content.substr(src + 5);
                                        auto end = content.find("\"");
                                        auto imageURL = content.substr(0, end);

                                        Log::writeLog("addresse" + imageURL);

                                        std::ofstream img;
                                        img.open(ARTICLE_FOLDER + "/" + imagePath);
                                        img << Util::getData(imageURL);
                                        img.close();

                                        auto toReplace = result.find(imageURL);

                                        if (toReplace != std::string::npos)
                                        {
                                            result.replace(toReplace, imageURL.length(), imagePath);
                                        }
                                    }
                                    else
                                    {
                                        Log::writeLog("Image does already exist.");
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }
                        catch (const std::exception &e)
                        {
                            Message(ICON_INFORMATION, "Information", e.what(), 1200);
                            Log::writeLog(e.what());
                        }

                        std::ofstream htmlfile;
                        htmlfile.open(path);
                        htmlfile << result;
                        htmlfile.close();
                    }
                    OpenBook(path.c_str(), "", 0);

                    _minifluxView->invertCurrentEntryColor();
                }
            }
            return 1;
        }
        else if (_currentView == Views::HNCOMMENTSVIEW)
        {
            if (_hnCommentView->checkIfEntryClicked(par1, par2))
            {
                _hnCommentView->invertCurrentEntryColor();
                ShowHourglassForce();

                if (_hnCommentView->getCurrentEntryItertator() == 0)
                {
                    if (_hnCommentView->getCurrentEntry()->parent != 0)
                    {
                        drawHN(_hnCommentView->getCurrentEntry()->parent);
                    }
                    else
                    {
                        drawMiniflux("", _minifluxViewShownPage);
                    }
                }
                else
                {
                    drawHN(_hnCommentView->getCurrentEntry()->id);
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

        if (_currentView == Views::MFVIEW)
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
        else if (_currentView == Views::HNCOMMENTSVIEW)
        {
            //go back one page
            if (par1 == 23)
            {
                ShowHourglassForce();
                if (_hnCommentView->getEntry(0)->parent != 0)
                {
                    drawHN(_hnCommentView->getEntry(0)->parent);
                }
                else
                {
                    drawMiniflux("", _minifluxViewShownPage);
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

void EventHandler::drawMiniflux(const string &filter, int page)
{

    if (_entries.size() > 0)
    {
        Log::writeLog(std::to_string(_entries.size()));
        _minifluxView = std::unique_ptr<MinifluxView>(new MinifluxView(_menu.getContentRect(), _entries, page));
        _hnCommentView.reset();
    }
    else
    {
        FillAreaRect(_menu.getContentRect(), WHITE);

        DrawTextRect2(_menu.getContentRect(), "no entries to show");
    }

    _currentView = Views::MFVIEW;
    FullUpdate();
}

void *EventHandler::getHnEntry(void *arg)
{
    try
    {
        HnEntry temp = Hackernews::getEntry(*(int *)arg);

    Log::writeLog("Got " + std::to_string(temp.id));

            pthread_mutex_lock(&mutexEntries);
            _eventHandlerStatic->_hnEntries.push_back(temp);
            pthread_mutex_unlock(&mutexEntries);
        }
    }
    catch (const std::exception &e)
    {
        Log::writeLogError(e.what());
    }

    return NULL;
}


void EventHandler::drawHN(int entryID)
{
    auto found = false;

    hnItem parentItem;
    std::vector<hnItem> currentHnComments;

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
        ShowHourglassForce();

        try
        {
            parentItem = Hackernews::getEntry(entryID);
        }
        catch (const std::exception &e)
        {
            Log::writeLogError(e.what());
            //change error msg
            Message(ICON_ERROR, "fuck", e.what(), 1200);

            return;
        }

        Util::decodeHTML(parentItem.text);
    }

    if (parentItem.kids.size() == 0)
    {
        Message(ICON_INFORMATION, "Info", "This Comment has no childs.", 1000);
        _minifluxView->invertEntryColor(_tempItemID);
    }
    else
    {
        if (parentItem.parent > 0 && !parentItem.text.empty())
            parentItem.text = "...";

        _hnItems.push_back(parentItem);
        currentHnComments.push_back(_hnItems.back());

        vector<int> tosearch;

        //test if items have already been downloaded
        for (size_t i = 0; i < parentItem.kids.size(); ++i)
        {
            found = false;

            for (size_t j = 0; j < _hnItems.size(); ++j)
            {
                if (parentItem.kids.at(i) == _hnItems.at(j).id)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                tosearch.push_back(parentItem.kids.at(i));
        }

        //Download comments
        mutexEntries = PTHREAD_MUTEX_INITIALIZER;
        int count;

        Util::connectToNetwork();

        auto counter = 0;

        //count of threads that can be handled
        auto threadsPerSession = 4;

        while (counter < tosearch.size())
        {
            if (counter % threadsPerSession == 0 || counter < threadsPerSession)
            {
                if ((tosearch.size() - counter) < threadsPerSession)
                    threadsPerSession = tosearch.size() - counter;

                pthread_t threads[threadsPerSession];

                for (count = 0; count < threadsPerSession; ++count)
                {
                    if (pthread_create(&threads[count], NULL, itemToEntries, &tosearch.at(counter)) != 0)
                    {
                        Log::writeLog("could not create thread");
                        break;
                    }
                    counter++;
                }

                for (size_t i = 0; i < count; ++i)
                {
                    if (pthread_join(threads[i], NULL) != 0)
                    {
                        Log::writeLog("cannot join thread" + std::to_string(i));
                    }
                }
            }
        }

        Log::writeLog("got childs");

        pthread_mutex_destroy(&mutexEntries);

        //TODO content modificaion

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

        Log::writeLog("start drawing");

        if (_hnCommentView != nullptr)
            _hnShownPage.insert(std::make_pair(_hnCommentView->getEntry(0)->id, _hnCommentView->getShownPage()));

        auto current = _hnShownPage.find(itemID);

        int page;

        if (current != _hnShownPage.end())
        {
            page = current->second;
            _hnShownPage.erase(itemID);
        }
        else
        {
            page = 1;
        }

        _minifluxView.reset();
        _hnCommentView.reset(new HnCommentView(_menu.getContentRect(), currentHnComments, page));

        PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);
    }
}
