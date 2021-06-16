//------------------------------------------------------------------
// eventHandler.h
//
// Author:           JuanJakob
// Date:             22.04.2021
// Description:      Handles all events and directs them
//-------------------------------------------------------------------

#ifndef EVENT_HANDLER
#define EVENT_HANDLER

#include "mainMenu.h"
#include "contextMenu.h"
#include "miniflux.h"
#include "minifluxView.h"
#include "util.h"
#include "hnCommentView.h"

#include <memory>
#include <map>
#include <vector>

const std::string CONFIG_FOLDER = "/mnt/ext1/system/config/miniflux";
const std::string CONFIG_PATH = CONFIG_FOLDER + "/miniflux.cfg";
const std::string ARTICLE_FOLDER = "/mnt/ext1/miniflux";

class EventHandler
{
public:
    /**
        * Defines fonds, sets global Event Handler and starts new content 
        */
    EventHandler();

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
    std::unique_ptr<ContextMenu> _contextMenu;
    std::unique_ptr<Miniflux> _miniflux;
    MainMenu _menu = MainMenu("Miniflux");
    int _tempItemID;
    std::vector<hnItem> _hnItems;
    std::vector<entry> _entries;
    int _minifluxViewShownPage = 1;
    std::map<int,int> _hnShownPage;

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
     * 
     * Called by the threads and writes items to _hnitems
     * 
     * @param void itemId that shall be downloaded
     */
    static void *itemToEntries(void *arg);

    /**
     * 
     * Draws the miniflux items to an ListView
     * 
     */
    void drawMiniflux(int page = 1);


    /**
     * 
     * Draws a HN Comment and its childs to an ListView
     * 
     * @param int parent itemId
     */
    void drawHN(int itemID);
};
#endif