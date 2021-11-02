//------------------------------------------------------------------
// eventHandler.h
//
// Author:           JuanJakob
// Date:             22.04.2021
// Description:      Handles all events and directs them
//-------------------------------------------------------------------

#ifndef EVENT_HANDLER
#define EVENT_HANDLER

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

#include "sqliteConnector.h"

#include <string>
#include <memory>
#include <map>
#include <vector>

enum Views
{
    DEFAULTVIEW,
    MFVIEW,
    HNCOMMENTSVIEW
};

const std::string CONFIG_FOLDER = "/mnt/ext1/system/config/miniflux";
const std::string CONFIG_PATH = CONFIG_FOLDER + "/miniflux.cfg";
const std::string ARTICLE_FOLDER = "/mnt/ext1/miniflux";
const std::string IMAGE_FOLDER = "/mnt/ext1/miniflux/img";
const std::string DB_PATH = ARTICLE_FOLDER + "/data.db";

class EventHandler
{
public:
    /**
        * Defines fonds, sets global Event Handler and starts new content 
        */
    EventHandler();

    ~EventHandler();

    /**
        * Handles events and redirects them
        * 
        * @param type event type
        * @param par1 first argument of the event
        * @param par2 second argument of the event
        * @return int returns if the event was handled
        */
    int eventDistributor(const int type, const int par1, const int par2);

private:
    static std::unique_ptr<EventHandler> _eventHandlerStatic;
    std::unique_ptr<MinifluxView> _minifluxView;
    std::unique_ptr<HnCommentView> _hnCommentView;
    std::unique_ptr<Miniflux> _miniflux;
    MainMenu _menu = MainMenu("Miniflux");
    HnContextMenu _hnContextMenu = HnContextMenu();
    ContextMenu _contextMenu = ContextMenu();
    SqliteConnector _sqliteCon = SqliteConnector(DB_PATH);
    std::vector<HnEntry> _hnEntries;
    Views _currentView;
    int _minifluxViewShownPage = 1;
    std::map<int, int> _hnShownPage;

    /**
        * Function needed to call C function, redirects to real function
        * 
        *  @param index int of the menu that is set
        */
    static void mainMenuHandlerStatic(const int index);

    /**
        * Handles menu events and redirects them
        * 
        * @param index int of the menu that is set
        */
    void mainMenuHandler(const int index);

    /**
        * Function needed to call C function, redirects to real function
        * 
        *  @param index int of the menu that is set
        */
    static void hnContextMenuHandlerStatic(const int index);

    /**
        * Handlescontext  menu events and redirects them
        * 
        * @param index int of the menu that is set
        */

    void hnContextMenuHandler(const int index);

    /**
        * Function needed to call C function, redirects to real function
        * 
        *  @param index int of the menu that is set
        */
    static void contextMenuHandlerStatic(const int index);

    /**
        * Handlescontext  menu events and redirects them
        * 
        * @param index int of the menu that is set
        */

    void contextMenuHandler(const int index);

    /**
        * Handles pointer Events
        * 
        * @param type event type
        * @param par1 first argument of the event
        * @param par2 second argument of the event
        * @return int returns if the event was handled
        */
    int pointerHandler(const int type, const int par1, const int par2);

    /**
        * Handles key Events
        * 
        * @param type event type
        * @param par1 first argument of the event (is the key)
        * @param par2 second argument of the event
        * @return int returns if the event was handled
        */
    int keyHandler(const int type, const int par1, const int par2);

    /**
        * create a html and store it
        * 
        * @param title name of the file
        * @param content content that shall be written to the html
        */
		void createHtml(std::string title, std::string content);

		bool drawMinifluxEntries(const std::vector<MfEntry> &mfEntries);
    /**
     * 
     * Draws the miniflux items to an ListView
     * 
     */
    void filterAndDrawMiniflux(const std::string &filter);

    void HnDownload(int entryID);

    /**
     * 
     * Called by the threads and writes items to _HnEntries
     * 
     * @param void EntryId that shall be downloaded
     */
    static void *getHnEntry(void *arg);

    /**
     * 
     * Draws a HN Comment and its childs to an ListView
     * 
     * @param int parent entryID
     */
    void drawHN(int entryID);

		void drawHnCommentView(const std::string &commentsURL);
};
#endif
