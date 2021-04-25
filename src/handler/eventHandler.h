//------------------------------------------------------------------
// eventHandler.h
//
// Author:           JuanJakob
// Date:             22.04.2021
// Description:      Handles all events and directs them
//-------------------------------------------------------------------

#ifndef EVENT_HANDLER
#define EVENT_HANDLER

#include "menuHandler.h"
#include "miniflux.h"
#include "listView.h"
#include "util.h"

#include <memory>

const string CONFIG_FOLDER = "/mnt/ext1/system/config/miniflux";
const string CONFIG_PATH = CONFIG_FOLDER + "/miniflux.cfg";

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
    std::unique_ptr<ListView> _listView;
    //std::unique_ptr<Miniflux> _miniflux;
    MenuHandler _menu = MenuHandler("Miniflux");

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
        * Handles pointer Events
        * 
        * @param type event type
        * @param par1 first argument of the event
        * @param par2 second argument of the event
        * @return int returns if the event was handled
        */
    int pointerHandler(const int type, const int par1, const int par2);
};
#endif