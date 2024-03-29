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

#include <algorithm>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <thread>
#include <mutex>

#include <fstream>
#include <sstream>

using std::string;
using std::vector;

std::unique_ptr<EventHandler> EventHandler::_eventHandlerStatic;
std::mutex mutexEntries;

EventHandler::EventHandler()
{
    //create an copy of the eventhandler to handle methods that require static functions
    //_eventHandlerStatic = std::make_unique<EventHandler>();
    _eventHandlerStatic = std::unique_ptr<EventHandler>(this);
    //static std::shared_ptr<EventHandler> _eventHandlerStatic;

    if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
    {
        if (iv_access(ARTICLE_FOLDER.c_str(), W_OK) != 0)
            iv_mkdir(ARTICLE_FOLDER.c_str(), 0666);

        _miniflux = std::make_unique<Miniflux>(Miniflux(Util::accessConfig(Action::IReadString,"url"), Util::accessConfig(Action::IReadSecret,"token")));

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

        _menu.draw();
    }
    else
    {
        Message(ICON_INFORMATION, "Config", ("Please create " + CONFIG_PATH + " containing username and token. Example is provided in the Repo.").c_str(), 5000);
        CloseApp();
    }

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
                auto curFrontLight = GetFrontlightState();
                Log::writeInfoLog(std::to_string(curFrontLight));
                SetFrontlightEnabled(0);
                OpenProgressbar(ICON_INFORMATION, "Syncing items", "Downloading Miniflux Entries", 0, NULL);
                vector<MfEntry> entriesToSync =  _sqliteCon.selectMfEntries(IsDownloaded::TOBEDOWNLOADED);

                double percentageMove = entriesToSync.size();
                percentageMove = 1/ percentageMove * 98;
                _currentPercentage = 0;
                for (const auto& ent : entriesToSync)
                {
                    std::thread imageDownloadThread;
                    try
                    {
                        _currentPercentage += percentageMove;
                        _downloadText = "Downloading \"" + ent.title + "\"";
                        UpdateProgressbar(_downloadText.c_str(), _currentPercentage);
                        if (ent.url.find("news.ycombinator.com") == std::string::npos)
                        {
                            imageDownloadThread = std::thread{Util::createHtml, ent.title, ent.content};
                        }
                        if (ent.comments_url.find("news.ycombinator.com") != std::string::npos)
                        {
                            _hnEntries.clear();
                            BanSleep(2000);

                            downloadHnEntries(getHnIDFromURL(ent.comments_url));

                            for(auto& item : _hnEntries)
                            {
                                item.mfEntryId = ent.id;
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
                    if(imageDownloadThread.joinable())
                        imageDownloadThread.join();
                }
                UpdateProgressbar("Updating downloaded items.", 99);
                _sqliteCon.deleteNotDownloadedMfEntries();

                vector<MfEntry> mfEntries = _sqliteCon.selectMfEntries(IsDownloaded::DOWNLOADED);
                for(auto& mfEntry : mfEntries)
                {
                    try{
                        mfEntry = _miniflux->getEntry(mfEntry.id);
                        _sqliteCon.updateMfEntry(mfEntry.id, mfEntry.starred, mfEntry.status);
                        //TODO also delete if not longer there?
                    }
                    catch (const std::exception &e)
                    {
                        Log::writeErrorLog(e.what());
                    }
                }
                mfEntries = _sqliteCon.selectMfEntries(IsDownloaded::DOWNLOADED);
                CloseProgressbar();
                SetFrontlightEnabled(1);
                drawMinifluxEntries(mfEntries);
                //no effect...
                //WiFiPower(0);
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
                string message;
                Message(ICON_INFORMATION, "Info",
                        message.append("Version ")
                            .append(PROGRAMVERSION)
                            .append("\nFor support please open a ticket at "
                                    "https://github.com/JuanJakobo/"
                                    "Pocketbook-Miniflux-Reader/issues")
                            .c_str(),
                        1200);
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

void EventHandler::sendToPocketKeyboardHandlerStatic(char *text)
{
    _eventHandlerStatic->sendToPocketKeyboardHandler(text);
}
void EventHandler::sendToPocketKeyboardHandler(char *text)
{
    if (!text)
        return;

    string s(text);
    if (s.empty())
        return;
    string number;
    std::istringstream ss(s);
    while (std::getline(ss, number, ','))
    {
        try
        {
            Log::writeInfoLog("Sending the following Url to Pocket: " + _hnCommentView->getCurrentEntry().urls.at(stoi(number)));
            //TODO return true/false
            _pocket->addItems(_hnCommentView->getCurrentEntry().urls.at(stoi(number)));
        }
        catch(std::exception e)
        {
            Log::writeErrorLog("Could not read in number of Url.");
            Log::writeErrorLog(e.what());
        }
    }
    HideHourglass();
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
                //why created at 0777?
                std::ofstream note(ARTICLE_FOLDER + "/notes", std::ios_base::app | std::ios_base::out);
                note << "by=" << _hnCommentView->getCurrentEntry().by << " id=" << _hnCommentView->getCurrentEntry().id << "\ncontent=\n" << _hnCommentView->getCurrentEntry().text << "\n\n";
                note.close();
                _hnCommentView->reDrawCurrentEntry();

                break;
            }
            //author
        case 102:
            {
                try{
                    Util::connectToNetwork();
                    HnUser user = Hackernews::getUser(_hnCommentView->getCurrentEntry().by);
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

                if(!_hnCommentView->getCurrentEntry().urls.empty())
                {
                    activatePocket();
                    _hnCommentView->invertCurrentEntryColor();
                    string text;
                    auto n = _hnCommentView->getCurrentEntry().urls.size();
                    for(decltype(n) i = 0; i < n; ++i)
                    {
                        text = text + '[' + std::to_string(i) + "] " + _hnCommentView->getCurrentEntry().urls[i] + "\n";
                        _keyboardText = std::to_string(i) + ',';

                    }

                    text = "When \"Send to Pocket\" is clicked, type in the numbers seperated by comma of the items that shall be synced. (e.g. 0,3,5)\n" + text;

                    auto dialogResult = DialogSynchro(ICON_INFORMATION, "Action",text.c_str(), "Send to Pocket", "Cancel", NULL);
                    if(dialogResult == 1)
                    {
                        _keyboardText.resize(60);
                        OpenKeyboard(_keyboardText.c_str(), &_keyboardText[0], 60 - 1, KBD_NORMAL, &sendToPocketKeyboardHandlerStatic);
                        break;
                    }
                }
                else
                {
                    Message(ICON_INFORMATION, "Info", "There are no Urls in this comment.", 1000);
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
        //openArticle Hackernews
        case 101:
            {
                openArticle();
                break;
            }
            //Mark/Unmark to download
        case 102:
            {
                if(_minifluxView->getCurrentEntry().downloaded == IsDownloaded::TOBEDOWNLOADED || _minifluxView->getCurrentEntry().downloaded == IsDownloaded::DOWNLOADED)
                {
                    _sqliteCon.deleteHnEntries(_minifluxView->getCurrentEntry().id);
                    string title_folder = Util::returnFolderName(_minifluxView->getCurrentEntry().title);
                    string cmd = "rm -rf \"" + title_folder + "\"";
                    system(cmd.c_str());
                    _minifluxView->getCurrentEntry().downloaded = IsDownloaded::NOTDOWNLOADED;
                }
                else if(_minifluxView->getCurrentEntry().downloaded == IsDownloaded::NOTDOWNLOADED)
                {
                    _minifluxView->getCurrentEntry().downloaded = IsDownloaded::TOBEDOWNLOADED;
                }
                _sqliteCon.updateDownloadStatusMfEntry(_minifluxView->getCurrentEntry().id, _minifluxView->getCurrentEntry().downloaded);
                _minifluxView->reDrawCurrentEntry();
                break;
            }
            //Unstar/Star
        case 103:
            {
                try{
                    MfEntry entry = _miniflux->getEntry(_minifluxView->getCurrentEntry().id);
                    _minifluxView->getCurrentEntry().status = entry.status;

                    if(entry.starred != _minifluxView->getCurrentEntry().starred)
                    {
                        _minifluxView->getCurrentEntry().starred = entry.starred;
                    }
                    else
                    {
                        _miniflux->toggleBookmark(_minifluxView->getCurrentEntry().id);
                        _minifluxView->getCurrentEntry().starred = !_minifluxView->getCurrentEntry().starred;
                    }
                    _sqliteCon.updateMfEntry(_minifluxView->getCurrentEntry().id, _minifluxView->getCurrentEntry().starred, entry.status);
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

            bool isHn = (_minifluxView->getCurrentEntry().comments_url.find("news.ycombinator.com") != std::string::npos) ? true : false;

            string downloaded;
            if(_minifluxView->getCurrentEntry().downloaded == IsDownloaded::TOBEDOWNLOADED)
                downloaded = "Remove download mark";
            else if(_minifluxView->getCurrentEntry().downloaded == IsDownloaded::NOTDOWNLOADED)
                downloaded = "Add download mark";
            else if(_minifluxView->getCurrentEntry().downloaded == IsDownloaded::DOWNLOADED)
                downloaded = "Remove item";

            _contextMenu.createMenu(par2, EventHandler::contextMenuHandlerStatic, isHn, _minifluxView->getCurrentEntry().starred, downloaded);
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
        if (IsInRect(par1, par2, &_menu.getMenuButtonRect()) == 1)
        {
            auto mainView = (_currentView == Views::HNCOMMENTSVIEW) ? false : true;

            return _menu.open(mainView, EventHandler::mainMenuHandlerStatic);
        }
        else if (_currentView == Views::MFVIEW)
        {
            if (_minifluxView->checkIfEntryClicked(par1, par2))
            {
                _minifluxView->invertCurrentEntryColor();

                if (_minifluxView->getCurrentEntry().comments_url.find("news.ycombinator.com") != std::string::npos)
                {
                    drawHnCommentView(_minifluxView->getCurrentEntry().comments_url);
                }
                else
                {
                    openArticle();
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
                    if (_hnCommentView->getCurrentEntry().parent != 0)
                    {
                        drawHN(_hnCommentView->getCurrentEntry().parent);
                    }
                    else
                    {
                        _minifluxView->draw();
                        _currentView = Views::MFVIEW;
                    }
                }
                else
                {
                    drawHN(_hnCommentView->getCurrentEntry().id);
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
                if (_hnCommentView->getEntry(0).parent != 0)
                {
                    drawHN(_hnCommentView->getEntry(0).parent);
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
        FillAreaRect(&_menu.getContentRect(), WHITE);
        DrawTextRect2(&_menu.getContentRect(), "no entries to show");
        _minifluxView.reset();
        _currentView = Views::DEFAULTVIEW;
        PartialUpdate(_menu.getContentRect().x, _menu.getContentRect().y, _menu.getContentRect().w, _menu.getContentRect().h);
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

void EventHandler::recursive(const HnEntry &parent)
{
    for(const auto id : parent.kids)
    {
    //TODO check if are in DB?
    //HnEntry temp = _sqliteCon.selectHnEntry(id);
    //if (temp.id == -1)
        try
        {
            HnEntry temp = Hackernews::getEntry(id);
            if (!temp.text.empty())
            {
                //TODO use constexpr?
                Util::decodeHTML(temp.text);
                auto found = temp.text.find("<a href=\"");
                while (found != std::string::npos)
                {
                    auto end3 = temp.text.find("</a>");

                    string toFind = temp.text.substr(found, (end3 + 4) - found);

                    auto url = toFind.substr(9);

                    auto src = url.find("\"");

                    url = url.substr(0, src);

                    temp.urls.push_back(url);

                    auto end1 = toFind.find(">");
                    auto end2 = toFind.find("</a>");

                    auto urlContent = toFind.substr(end1 + 1, end2 - end1 - 1);

                    auto toReplace = temp.text.find(toFind);

                    if (toReplace != std::string::npos)
                    {
                        temp.text.replace(toReplace, toFind.size(), urlContent);
                    }

                    found = temp.text.find("<a href=\"");
                }
            }
            _hnEntries.push_back(temp);
            recursive(temp);
        }
        catch (const std::exception &e)
        {
            Log::writeErrorLog(e.what());
        }
    }
}

//TODO recursive or thread?
void EventHandler::downloadHnEntries(int parentCommentItemID)
{
    //_sqliteCon.selectHnEntries(entryID, _hnEntries);

    HnEntry parentItem = _sqliteCon.selectHnEntry(parentCommentItemID);
    if (parentItem.id == -1)
    {
        Util::connectToNetwork();
        parentItem = Hackernews::getEntry(parentCommentItemID);
        if(!parentItem.title.empty())
            Util::decodeHTML(parentItem.title);
        if(!parentItem.text.empty())
            Util::decodeHTML(parentItem.text);
        _hnEntries.push_back(parentItem);
    }
    string text = _downloadText + " (" + std::to_string(parentItem.descendants) + " comments)";

    UpdateProgressbar(text.c_str(), _currentPercentage);
    recursive(parentItem);
}

void EventHandler::getHnEntry(int commentID)
{
    try
    {
        HnEntry temp = Hackernews::getEntry(commentID);
        if (!temp.text.empty())
        {
            //TODO use constexpr?
            Util::decodeHTML(temp.text);
            auto found = temp.text.find("<a href=\"");
            while (found != std::string::npos)
            {
                auto end3 = temp.text.find("</a>");

                string toFind = temp.text.substr(found, (end3 + 4) - found);

                auto url = toFind.substr(9);

                auto src = url.find("\"");

                url = url.substr(0, src);

                temp.urls.push_back(url);

                auto end1 = toFind.find(">");
                auto end2 = toFind.find("</a>");

                auto urlContent = toFind.substr(end1 + 1, end2 - end1 - 1);

                auto toReplace = temp.text.find(toFind);

                if (toReplace != std::string::npos)
                {
                    temp.text.replace(toReplace, toFind.size(), urlContent);
                }

                found = temp.text.find("<a href=\"");
            }
        }
        mutexEntries.lock();
        _eventHandlerStatic->_hnEntries.push_back(temp);
        mutexEntries.unlock();
    }
    catch (const std::exception &e)
    {
        Log::writeErrorLog(e.what());
    }
}

//TODO MOVEEEE to own class and make shorter...

void EventHandler::drawHN(int entryID)
{
    std::vector<HnEntry> currentHnComments;

    _hnEntries.clear();

    //test if all are there?
    HnEntry parentItem = _sqliteCon.selectHnEntry(entryID);
    if (parentItem.id == -1)
    {
        if(DialogSynchro(ICON_INFORMATION, "Action","Comment are not downloaded. This might take a while.", "Download", "Cancel", nullptr) == 1)
        {
            downloadHnEntries(entryID);
        }
        else
        {
            _minifluxView->invertCurrentEntryColor();
            return;
        }
    }

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
        _sqliteCon.selectHnEntries(entryID, _hnEntries);
        if (parentItem.parent > 0 && !parentItem.text.empty())
            parentItem.text = "...";

        currentHnComments.push_back(parentItem);

        //TODO why?
        for (size_t i = 0; i < parentItem.kids.size(); ++i)
        {
            for (size_t j = 0; j < _hnEntries.size(); ++j)
            {
                if (parentItem.kids.at(i) == _hnEntries.at(j).id)
                {
                    currentHnComments.push_back(_hnEntries.at(j));
                }
            }
        }

        if (_hnCommentView != nullptr)
            _hnShownPage.insert(std::make_pair(_hnCommentView->getEntry(0).id, _hnCommentView->getShownPage()));

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

        //TODO calculate each time newly...
        //have a vector of comment views?
        _hnCommentView.reset(new HnCommentView(_menu.getContentRect(), currentHnComments, page));
        _currentView = Views::HNCOMMENTSVIEW;
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


void EventHandler::openArticle()
{
                    string excerpt = _minifluxView->getCurrentEntry().content;

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
                                if (_minifluxView->getCurrentEntry().reading_time == 0)
                                {
                                    Message(ICON_INFORMATION, "Info", "The content of the article has not been provided by miniflux.", 1000);
                                }
                                else
                                {

                                    //TODO change...
                                    auto path = Util::createHtml(_minifluxView->getCurrentEntry().title, _minifluxView->getCurrentEntry().content);
                                    OpenBook(path.c_str(), "", 0);
                                }
                                break;
                            }
                        case 2:
                            {
                                activatePocket();
                                _pocket->addItems(_minifluxView->getCurrentEntry().url);
                                HideHourglass();
                            break;
                            }
                        default:
                            break;
                    }
                    _minifluxView->invertCurrentEntryColor();
}


void EventHandler::activatePocket()
{
    if(!_pocket)
    {
        if (iv_access(CONFIG_PATH.c_str(), W_OK) == 0)
        {
            auto accessToken = Util::accessConfig(Action::IReadSecret,"AccessToken");
            if(!accessToken.empty())
                _pocket = std::make_unique<Pocket>();
        }
        else
        {
            int dialogResult = DialogSynchro(ICON_INFORMATION, "Action","Do you want to activate Pocket?", "Yes", "No", "Cancel");
            if (dialogResult == 1)
            {
                _pocket = std::make_unique<Pocket>();
            }
        }

    }
}
