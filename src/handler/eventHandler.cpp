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

#include "pocket.h"

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

#include <fstream>

using std::string;
using std::vector;

std::unique_ptr<EventHandler> EventHandler::_eventHandlerStatic;
pthread_mutex_t mutexEntries;

EventHandler::EventHandler()
{
    //create an copy of the eventhandler to handle methods that require static functions
    _eventHandlerStatic = std::unique_ptr<EventHandler>(this);

    if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
    {
        if (iv_access(ARTICLE_FOLDER.c_str(), W_OK) != 0)
            iv_mkdir(ARTICLE_FOLDER.c_str(), 0777);

        if (iv_access(IMAGE_FOLDER.c_str(), W_OK) != 0)
            iv_mkdir(IMAGE_FOLDER.c_str(), 0777);

        _miniflux = std::unique_ptr<Miniflux>(new Miniflux(Util::accessConfig(Action::IReadString,"url"), Util::accessConfig(Action::IReadSecret,"token")));

        vector<MfEntry> mfEntries = _sqliteCon.selectMfEntries(IsDownloaded::DOWNLOADED);
        vector<MfEntry> toBeDownloaded = _sqliteCon.selectMfEntries(IsDownloaded::TOBEDOWNLOADED);
        mfEntries.insert(mfEntries.end(),toBeDownloaded.begin(),toBeDownloaded.end());

        if(!drawMinifluxEntries(mfEntries))
        {
            string filter = Util::accessConfig(Action::IReadString,"filter");
            if (filter.empty())
            {
                filter = "status=unread&direction=asc&limit=1000";
                Util::accessConfig(Action::IWriteString, "filter", filter);
            }
            filterAndDrawMiniflux(filter);
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
    Log::writeInfoLog("delete eventHandler");
}

int EventHandler::eventDistributor(const int type, const int par1, const int par2)
{
    if (ISPOINTEREVENT(type))
        return EventHandler::pointerHandler(type, par1, par2);
    else if (ISKEYEVENT(type))
        return EventHandler::keyHandler(type, par1, par2);

    return 1;
}

void EventHandler::mainMenuHandlerStatic(const int index)
{
    _eventHandlerStatic->mainMenuHandler(index);
}

void EventHandler::mainMenuHandler(const int index)
{
    switch (index)
    {
        //show downloaded
        case 101:
            {
                vector<MfEntry> mfEntries = _sqliteCon.selectMfEntries(IsDownloaded::DOWNLOADED);
                vector<MfEntry> toBeDownloaded = _sqliteCon.selectMfEntries(IsDownloaded::TOBEDOWNLOADED);
                mfEntries.insert(mfEntries.end(),toBeDownloaded.begin(),toBeDownloaded.end());
                drawMinifluxEntries(mfEntries);
                break;
            }
            //show unread
        case 102:
            {
                filterAndDrawMiniflux(Util::accessConfig(Action::IReadString,"filter"));
                break;
            }
            //show starred
        case 103:
            {
                filterAndDrawMiniflux("starred=true&direction=asc&limit=1000");
                break;
            }
            //sync items
        case 104:
            {
                OpenProgressbar(ICON_INFORMATION, "Syncing items", "Downloading Miniflux Entries", 0, NULL);
                vector<MfEntry> entriesToSync =  _sqliteCon.selectMfEntries(IsDownloaded::TOBEDOWNLOADED);

                double percentageMove = entriesToSync.size();
                percentageMove = 1/ percentageMove * 98;
                auto currentPercentage = 0;
                for (auto ent : entriesToSync)
                {
                    try
                    {
                        currentPercentage += percentageMove;
                        UpdateProgressbar(("Downloading \"" + ent.title + "\"").c_str(), currentPercentage);
                        if (ent.url.find("news.ycombinator.com") == std::string::npos)
                            Util::createHtml(ent.title, ent.content);
                        if (ent.comments_url.find("news.ycombinator.com") != std::string::npos)
                        {
                            _hnEntries.clear();
                            BanSleep(2000);
                            downloadHnEntries(getHnIDFromURL(ent.comments_url));

                            for(size_t i = 0; i < _hnEntries.size(); ++i)
                            {
                                _hnEntries.at(i).mfEntryId = ent.id;
                            }
                            _sqliteCon.insertHnEntries(_hnEntries);
                        }

                        _sqliteCon.updateDownloadStatusMfEntry(ent.id,IsDownloaded::DOWNLOADED);
                    }
                    catch (const std::exception &e)
                    {
                        Log::writeErrorLog(e.what());
                        Message(ICON_INFORMATION, "Error while downloading", e.what(), 1200);
                    }
                }
                UpdateProgressbar("Updating downloaded items.", 99);
                vector<MfEntry> mfEntries = _sqliteCon.selectMfEntries(IsDownloaded::DOWNLOADED);
                for (size_t i = 0; i < mfEntries.size(); i++)
                {
                    try{
                        mfEntries.at(i) = _miniflux->getEntry(mfEntries.at(i).id);
                        _sqliteCon.updateMfEntry(mfEntries.at(i).id, mfEntries.at(i).starred, mfEntries.at(i).status);
                    }
                    catch (const std::exception &e)
                    {
                        Log::writeErrorLog(e.what());
                    }
                }
                mfEntries = _sqliteCon.selectMfEntries(IsDownloaded::DOWNLOADED);

                CloseProgressbar();
                drawMinifluxEntries(mfEntries);
                break;
            }
            //Mark as read till page
        case 105:
            {
                try{
                    if(_currentView == Views::MFVIEW){
                        _miniflux->updateEntries(_minifluxView->getEntriesTillPage(), true);
                        filterAndDrawMiniflux(Util::accessConfig(Action::IReadString,"filter"));
                    }
                    else
                    {
                        Message(ICON_ERROR, "Error", "Could not mark entries as read", 1200);
                    }
                }
                catch (const std::exception &e)
                {
                    Log::writeErrorLog(e.what());
                    Message(ICON_ERROR, "Error", e.what(), 1200);
                }
                break;
            }
            //Go back to miniflux overview
        case 106:
            {
                _minifluxView->draw();
                _currentView = Views::MFVIEW;
                break;
            }
        case 107:
            {
                Message(ICON_INFORMATION, "Info", "Info", 1200);
                break;
            }
            //Exit
        case 108:
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
                std::ofstream note(ARTICLE_FOLDER + "/notes.txt", std::ios_base::app | std::ios_base::out);
                note << "by=" << _hnCommentView->getCurrentEntry()->by << " id=" << _hnCommentView->getCurrentEntry()->id << "\ncontent=\n" << _hnCommentView->getCurrentEntry()->text << "\n\n";
                note.close();
                _hnCommentView->reDrawCurrentEntry();

                break;
            }
            //author
        case 102:
            {
                try{
                    Util::connectToNetwork();
                    HnUser user = Hackernews::getUser(_hnCommentView->getCurrentEntry()->by);
                    Util::decodeHTML(user.about);
                    string message = "User: " + user.id + "\n Karma: " + std::to_string(user.karma) + "\n About: " + user.about + "\n Created: " + std::to_string(user.created);
                    DialogSynchro(ICON_INFORMATION, "User information", message.c_str(), "Close", NULL, NULL);
                    _hnCommentView->reDrawCurrentEntry();
                }
                catch (const std::exception &e)
                {
                    Log::writeErrorLog(e.what());
                    Message(ICON_ERROR, "Error", e.what(), 1200);
                }
                break;
            }
            //urls
        case 103:
            {
                if(!_hnCommentView->getCurrentEntry()->urls.empty())
                {
                    string text;
                    for(int i = 0; i < _hnCommentView->getCurrentEntry()->urls.size();i++)
                    {
                        text = text + '[' + std::to_string(i+1) + "] " + _hnCommentView->getCurrentEntry()->urls[i] + "\n";
                    }
                    int dialogResult = DialogSynchro(ICON_INFORMATION, "Action",text.c_str(), "Send to Pocket", "Cancel", NULL);
                    switch (dialogResult)
                    {
                        case 1:
                            //TODO type in numbers that should be saved to pocket
                            //TODO show urls off the command and make clickable,  also in dialog, can then be clicked?
                            // open hackernews in comments again
                            break;
                        default:
                            break;
                    }
                }
            }
        default:
            {
                _hnCommentView->invertCurrentEntryColor();
                break;
            }
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
        //Comment
        case 101:
            {
                drawHnCommentView(_minifluxView->getCurrentEntry()->comments_url);
                break;
            }
            //Mark/Unmark to download
        case 102:
            {
                if(_minifluxView->getCurrentEntry()->downloaded == IsDownloaded::TOBEDOWNLOADED || _minifluxView->getCurrentEntry()->downloaded == IsDownloaded::DOWNLOADED)
                {
                    _sqliteCon.deleteHnEntries(_minifluxView->getCurrentEntry()->id);


                    string title = Util::clearString(_minifluxView->getCurrentEntry()->title);

                    string path = ARTICLE_FOLDER + "/" + title + ".html";
                    remove(path.c_str());
                    string cmd = "rm -rf \"" + ARTICLE_FOLDER + "/img/" + title + "/\"";
                    system(cmd.c_str());
                    _minifluxView->getCurrentEntry()->downloaded = IsDownloaded::NOTDOWNLOADED;
                }
                else if(_minifluxView->getCurrentEntry()->downloaded == IsDownloaded::NOTDOWNLOADED)
                {
                    _minifluxView->getCurrentEntry()->downloaded = IsDownloaded::TOBEDOWNLOADED;
                }
                _sqliteCon.updateDownloadStatusMfEntry(_minifluxView->getCurrentEntry()->id, _minifluxView->getCurrentEntry()->downloaded);
                _minifluxView->reDrawCurrentEntry();
                break;
            }
            //Unstar/Star
        case 103:
            {
                try{
                    MfEntry entry = _miniflux->getEntry(_minifluxView->getCurrentEntry()->id);
                    _minifluxView->getCurrentEntry()->status = entry.status;

                    if(entry.starred != _minifluxView->getCurrentEntry()->starred)
                    {
                        _minifluxView->getCurrentEntry()->starred = entry.starred;
                    }
                    else
                    {
                        _miniflux->toggleBookmark(_minifluxView->getCurrentEntry()->id);
                        _minifluxView->getCurrentEntry()->starred = !_minifluxView->getCurrentEntry()->starred;
                    }
                    _sqliteCon.updateMfEntry(_minifluxView->getCurrentEntry()->id, _minifluxView->getCurrentEntry()->starred, entry.status);
                    _minifluxView->reDrawCurrentEntry();
                }
                catch (const std::exception &e)
                {
                    Log::writeErrorLog(e.what());
                    Message(ICON_ERROR, "Error", e.what(), 1200);
                    _minifluxView->invertCurrentEntryColor();
                }
                HideHourglass();
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

            string downloaded;
            if(_minifluxView->getCurrentEntry()->downloaded == IsDownloaded::TOBEDOWNLOADED) 
                downloaded = "Remove download mark";
            else if(_minifluxView->getCurrentEntry()->downloaded == IsDownloaded::NOTDOWNLOADED)
                downloaded = "Add download mark";
            else if(_minifluxView->getCurrentEntry()->downloaded == IsDownloaded::DOWNLOADED)
                downloaded = "Remove item";

            _contextMenu.createMenu(par2, EventHandler::contextMenuHandlerStatic, comments, _minifluxView->getCurrentEntry()->starred, downloaded);
            return 0;
        }
        else if (_currentView == Views::HNCOMMENTSVIEW)
        {
            if (_hnCommentView->checkIfEntryClicked(par1, par2))
            {
                _hnCommentView->invertCurrentEntryColor();
                _hnContextMenu.createMenu(par2, EventHandler::hnContextMenuHandlerStatic);
                return 0;
            }
        }
    }
    else if (type == EVT_POINTERUP)
    {
        //if menu is clicked
        if (IsInRect(par1, par2, _menu.getMenuButtonRect()) == 1)
        {
            auto mainView = true;
            if (_currentView == Views::HNCOMMENTSVIEW)
                mainView = false;

            return _menu.createMenu(mainView, EventHandler::mainMenuHandlerStatic);
        }
        else if (_currentView == Views::MFVIEW)
        {
            if (_minifluxView->checkIfEntryClicked(par1, par2))
            {
                _minifluxView->invertCurrentEntryColor();
                if (_minifluxView->getCurrentEntry()->url.find("news.ycombinator.com") != std::string::npos)
                {
                    drawHnCommentView(_minifluxView->getCurrentEntry()->comments_url);
                }
                else
                {

                    string excerpt = _minifluxView->getCurrentEntry()->content;

                    Util::replaceAll(excerpt,"\n", "");
                    Util::decodeHTML(excerpt);

                    auto found = excerpt.find('<');
                    while (found != std::string::npos)
                    {
                        auto end = excerpt.find('>');
                        if(end != std::string::npos)
                        {
                            auto replaceString = excerpt.substr(found, (end + 1) - found);

                            if(!replaceString.empty())
                            {
                                auto replaceBegin = excerpt.find(replaceString);
                                if (replaceBegin != std::string::npos)
                                    excerpt.replace(replaceBegin, replaceString.size(), "");
                                found = excerpt.find("<");
                            }
                            else
                            {
                                found = std::string::npos;
                            }
                        }
                        else
                        {
                            found = std::string::npos;
                        }
                    }

                    if(excerpt.size() > 800)
                        excerpt = excerpt.substr(0,800);
                    else if(excerpt.empty())
                        excerpt = "no excerpt";

                    int dialogResult = DialogSynchro(ICON_INFORMATION, "Action",("Excerpt\n" + excerpt + "...").c_str(), "Read article", "Send to Pocket", "Cancel");
                    switch (dialogResult)
                    {
                        case 1:
                            {
                                if (_minifluxView->getCurrentEntry()->reading_time == 0)
                                {
                                    Message(ICON_INFORMATION, "Info", "The content of the article has not been provided by miniflux.", 1000);
                                }
                                else
                                {

                                    auto path = Util::createHtml(_minifluxView->getCurrentEntry()->title, _minifluxView->getCurrentEntry()->content);
                                    OpenBook(path.c_str(), "", 0);
                                }
                                break;
                            }
                        case 2:
                            {
                               Pocket _pocket = Pocket();
                                _pocket.addItems(_minifluxView->getCurrentEntry()->url);
                                HideHourglass();
                            break;
                            }
                        default:
                            break;
                    }
                    _minifluxView->invertCurrentEntryColor();
                }
            }
            return 0;
        }
        else if (_currentView == Views::HNCOMMENTSVIEW)
        {
            if (_hnCommentView->checkIfEntryClicked(par1, par2))
            {
                _hnCommentView->invertCurrentEntryColor();

                if (_hnCommentView->getCurrentEntryItertator() == 0)
                {
                    if (_hnCommentView->getCurrentEntry()->parent != 0)
                    {
                        drawHN(_hnCommentView->getCurrentEntry()->parent);
                    }
                    else
                    {
                        _minifluxView->draw();
                        _currentView = Views::MFVIEW;
                    }
                }
                else
                {
                    drawHN(_hnCommentView->getCurrentEntry()->id);
                }
            }
            return 0;
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
                return 0;
            }
            //left button -> pre page
            else if (par1 == 24)
            {
                _minifluxView->prevPage();
                return 0;
            }
            //right button -> next page
            else if (par1 == 25)
            {
                _minifluxView->nextPage();
                return 0;
            }
        }
        else if (_currentView == Views::HNCOMMENTSVIEW)
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
                    _minifluxView->draw();
                    _currentView = Views::MFVIEW;
                }
                return 0;
            }
            //left button -> pre page
            else if (par1 == 24)
            {
                _hnCommentView->prevPage();
                return 0;
            }
            //right button -> next page
            else if (par1 == 25)
            {
                _hnCommentView->nextPage();
                return 0;
            }
        }
    }

    return 1;
}

bool EventHandler::drawMinifluxEntries(const vector<MfEntry> &mfEntries)
{
    if (mfEntries.size() > 0)
    {
        _minifluxView.reset(new MinifluxView(_menu.getContentRect(),mfEntries,1));
        _minifluxView->draw();
        _currentView = Views::MFVIEW;
        return true;
    }
    else
    {
        FillAreaRect(_menu.getContentRect(), WHITE);
        DrawTextRect2(_menu.getContentRect(), "no entries to show");
        _minifluxView.reset();
        _currentView = Views::DEFAULTVIEW;
        PartialUpdate(_menu.getContentRect()->x, _menu.getContentRect()->y, _menu.getContentRect()->w, _menu.getContentRect()->h);
        return false;
    }

}

void EventHandler::filterAndDrawMiniflux(const string &filter)
{
    if (!filter.empty())
    {
        try{
            vector<MfEntry> mfEntries = _miniflux->getEntries(filter);
            vector<MfEntry> oldEntries = _sqliteCon.selectMfEntries();
            for(size_t i = 0; i < mfEntries.size(); i++)
            {
                for(size_t j = 0; j < oldEntries.size();j++)
                {
                    if(mfEntries.at(i).id == oldEntries.at(j).id)
                    {
                        mfEntries.at(i).downloaded = oldEntries.at(j).downloaded;
                        break;
                    }
                }
            }
            _sqliteCon.insertMfEntries(mfEntries);
            drawMinifluxEntries(mfEntries);
        }
        catch (const std::exception &e)
        {
            Log::writeErrorLog(e.what());
            Message(ICON_ERROR, "Error", e.what(), 1200);
        }
    }
}

HnEntry EventHandler::HnDownload(int entryID)
{
    vector<HnEntry> temp = _sqliteCon.selectHnEntries(entryID);

    for (size_t i = 0; i < temp.size(); i++)
    {
        _hnEntries.push_back(temp.at(i));
    }

    HnEntry test = _sqliteCon.selectHnEntry(entryID);
    if (test.id > 0)
        _hnEntries.push_back(test);

    auto found = false;
    HnEntry parentItem;

    for (size_t i = 0; i < _hnEntries.size(); i++)
    {
        if (_hnEntries.at(i).id == entryID)
        {
            parentItem = _hnEntries.at(i);
            found = true;
            break;
        }
    }

    if (!found)
    {
        Util::connectToNetwork();
        parentItem = Hackernews::getEntry(entryID);
        if(!parentItem.title.empty())
            Util::decodeHTML(parentItem.title);
        if(!parentItem.text.empty())
            Util::decodeHTML(parentItem.text);
        _hnEntries.push_back(parentItem);

    }
    if (parentItem.kids.size() > 0)
    {
        vector<int> tosearch;

        for (size_t i = 0; i < parentItem.kids.size(); ++i)
        {
            found = false;

            for (size_t j = 0; j < _hnEntries.size(); ++j)
            {
                if (parentItem.kids.at(i) == _hnEntries.at(j).id)
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                tosearch.push_back(parentItem.kids.at(i));
        }

        if (tosearch.size() > 0)
        {
            Util::connectToNetwork();
            //Download comments
            mutexEntries = PTHREAD_MUTEX_INITIALIZER;
            int count;
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
                        if (pthread_create(&threads[count], NULL, getHnEntry, &tosearch.at(counter)) != 0)
                        {
                            Log::writeErrorLog("could not create thread");
                            break;
                        }
                        counter++;
                    }

                    for (size_t i = 0; i < count; ++i)
                    {
                        if (pthread_join(threads[i], NULL) != 0)
                        {
                            Log::writeErrorLog("cannot join thread" + std::to_string(i));
                        }
                    }
                }
            }

            pthread_mutex_destroy(&mutexEntries);
        }

    }
    return parentItem;
}

void EventHandler::downloadHnEntries(int parentCommentItemID)
{
    auto parentItem = HnDownload(parentCommentItemID);
    for (size_t i = 0; i < parentItem.kids.size(); ++i)
    {
        downloadHnEntries(parentItem.kids.at(i));
    }
}

void *EventHandler::getHnEntry(void *arg)
{
    try
    {
        HnEntry temp = Hackernews::getEntry(*(int *)arg);

        if (!temp.text.empty())
        {
            Util::decodeHTML(temp.text);
            auto found = temp.text.find("<a href=\"");
            while (found != std::string::npos)
            {
                //TODO also needed for header(ask hn etc.)
                auto end3 = temp.text.find("</a>");

                string toFind = temp.text.substr(found, (end3 + 4) - found);

                auto url = toFind.substr(9);

                auto src = url.find("\"");

                url = url.substr(0, src);

                temp.urls.push_back(url);

                auto end1 = toFind.find(">");
                auto end2 = toFind.find("</a>");

                auto imageURL = toFind.substr(end1 + 1, end2 - end1 - 1);

                auto toReplace = temp.text.find(toFind);

                if (toReplace != std::string::npos)
                {
                    temp.text.replace(toReplace, toFind.size(), imageURL);
                }

                found = temp.text.find("<a href=\"");
            }
        }

        pthread_mutex_lock(&mutexEntries);
        _eventHandlerStatic->_hnEntries.push_back(temp);
        pthread_mutex_unlock(&mutexEntries);
    }
    catch (const std::exception &e)
    {
        Log::writeErrorLog(e.what());
    }


    return NULL;
}

void EventHandler::drawHN(int entryID)
{
    std::vector<HnEntry> currentHnComments;

    _hnEntries.clear();
    auto parentItem = HnDownload(entryID);

    if (parentItem.kids.size() == 0)
    {
        Message(ICON_INFORMATION, "Info", "This Comment has no childs.", 1000);
        if (_currentView == Views::HNCOMMENTSVIEW)
            _hnCommentView->invertCurrentEntryColor();
        else
            _minifluxView->invertCurrentEntryColor();
    }
    else
    {
        if (parentItem.parent > 0 && !parentItem.text.empty())
            parentItem.text = "...";

        currentHnComments.push_back(parentItem);

        for (size_t i = 0; i < parentItem.kids.size(); ++i)
        {
            for (size_t j = 0; j < _hnEntries.size(); ++j)
            {
                if (parentItem.kids.at(i) == _hnEntries.at(j).id)
                {
                    if (!_hnEntries.at(j).deleted)
                        currentHnComments.push_back(_hnEntries.at(j));
                    break;
                }
            }
        }

        if (currentHnComments.size() == 0)
        {
            Message(ICON_INFORMATION, "Info", "All comments are deleted", 1000);
            _hnCommentView->invertCurrentEntryColor();
        }
        else
        {
            if (_hnCommentView != nullptr)
                _hnShownPage.insert(std::make_pair(_hnCommentView->getEntry(0)->id, _hnCommentView->getShownPage()));

            auto current = _hnShownPage.find(entryID);
            int page;

            if (current != _hnShownPage.end())
            {
                page = current->second;
                _hnShownPage.erase(entryID);
            }
            else
            {
                page = 1;
            }

            _hnCommentView.reset(new HnCommentView(_menu.getContentRect(), &currentHnComments, page));
            _currentView = Views::HNCOMMENTSVIEW;
        }
    }
}

void EventHandler::drawHnCommentView(const string &commentsURL)
{
    _minifluxViewShownPage = _minifluxView->getShownPage();
    _hnEntries.clear();
    drawHN(getHnIDFromURL(commentsURL));
}

int EventHandler::getHnIDFromURL(const string &url)
{
    auto parentCommentItemID = url;
    auto end = parentCommentItemID.find("id=");
    parentCommentItemID = parentCommentItemID.substr(end + 3);
    return std::stoi(parentCommentItemID);
}
