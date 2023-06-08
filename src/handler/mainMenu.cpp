//------------------------------------------------------------------
// mainMenu.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------

#include "mainMenu.h"

#include <string>

#include "inkview.h"

namespace
{
constexpr auto SYNC_DOWNLOAD_TITLE{"Sync marked"};
constexpr auto SHOW_UNREAD_TITLE{"Show unread"};
constexpr auto SHOW_STARRED_TITLE{"Show starred"};
constexpr auto SHOW_DOWNLOADED_TITLE{"Show downloaded"};
constexpr auto MARK_AS_READ_TILL_PAGE_TITLE{"Mark as read till page"};
constexpr auto GO_BACK_TO_OVERVIEW_TITLE{"Go back to overview"};
constexpr auto EXIT_TITLE{"Close App"};
constexpr auto INFO_TITLE{"Info"};
constexpr auto MENU_TITLE{"Menu"};

constexpr auto MENU_BUTTON_WIDTH_SCALE{2};
constexpr auto MENU_POSITION{0u};

constexpr auto SCREEN_BEGIN{0u};
constexpr auto MENU_PANEL_HEIGHT_SCALE{18};
constexpr auto MENU_PANEL_WIDTH_SCALE{3};

constexpr auto PANEL_OFF{0u};

constexpr auto TEXT_MARGIN{10};
constexpr auto TEXT_BEGIN{SCREEN_BEGIN + TEXT_MARGIN};

constexpr auto MENU_FONT{"LiberationMono-Bold"};
constexpr auto HEADER{0};
} // namespace

MainMenu::MainMenu(const std::string &p_name) : m_name{p_name}
{
    auto menuPanelHeight{ScreenHeight() / MENU_PANEL_HEIGHT_SCALE};
    auto menuPanelWidth{ScreenWidth() / MENU_PANEL_WIDTH_SCALE};
    auto menuPanelBeginX{ScreenWidth() - menuPanelWidth};
    m_menuRect = iRect(menuPanelBeginX, SCREEN_BEGIN, menuPanelWidth, menuPanelHeight, ALIGN_CENTER);

    auto menuButtonRectWidth{menuPanelWidth * MENU_BUTTON_WIDTH_SCALE};
    m_menuButtonRect = iRect(menuButtonRectWidth, m_menuRect.y, m_menuRect.w, m_menuRect.h, ALIGN_RIGHT);

    auto contentRectHeight{ScreenHeight() - menuPanelHeight};
    auto textEnd{ScreenWidth() - (TEXT_MARGIN * 2)};
    m_contentRect = iRect(TEXT_BEGIN, menuPanelHeight, textEnd, contentRectHeight, ALIGN_FIT);

    SetPanelType(PANEL_OFF);
}

void MainMenu::draw() const
{
    auto menuPanelFontSize{m_menuRect.h / 2};
    auto menuFont{OpenFont(MENU_FONT, menuPanelFontSize, FONT_STD)};
    SetFont(menuFont, BLACK);
    DrawTextRect(SCREEN_BEGIN, m_menuRect.y, ScreenWidth(), m_menuRect.h, m_name.c_str(), ALIGN_CENTER);
    DrawTextRect2(&m_menuButtonRect, MENU_TITLE);
    CloseFont(menuFont);

    auto lineThickness{m_menuRect.h - 1};
    DrawLine(SCREEN_BEGIN, lineThickness, ScreenWidth(), lineThickness, BLACK);
    PartialUpdate(SCREEN_BEGIN, m_menuRect.y, ScreenWidth(), m_menuRect.h);
}

int MainMenu::open(bool p_mainView, const iv_menuhandler &p_handler) const
{

    imenu mainMenu[] = {{ITEM_HEADER, HEADER, const_cast<char *>(MENU_TITLE), NULL},
                        {p_mainView ? static_cast<short>(ITEM_ACTIVE) : static_cast<short>(ITEM_HIDDEN), 101,
                         const_cast<char *>(SHOW_DOWNLOADED_TITLE), NULL},
                        {p_mainView ? static_cast<short>(ITEM_ACTIVE) : static_cast<short>(ITEM_HIDDEN), 102,
                         const_cast<char *>(SHOW_UNREAD_TITLE), NULL},
                        {p_mainView ? static_cast<short>(ITEM_ACTIVE) : static_cast<short>(ITEM_HIDDEN), 103,
                         const_cast<char *>(SHOW_STARRED_TITLE), NULL},
                        {p_mainView ? static_cast<short>(ITEM_ACTIVE) : static_cast<short>(ITEM_HIDDEN), 104,
                         const_cast<char *>(SYNC_DOWNLOAD_TITLE), NULL},
                        {p_mainView ? static_cast<short>(ITEM_ACTIVE) : static_cast<short>(ITEM_HIDDEN), 105,
                         const_cast<char *>(MARK_AS_READ_TILL_PAGE_TITLE), NULL},
                        {p_mainView ? static_cast<short>(ITEM_HIDDEN) : static_cast<short>(ITEM_ACTIVE), 106,
                         const_cast<char *>(GO_BACK_TO_OVERVIEW_TITLE), NULL},
                        {ITEM_ACTIVE, 107, const_cast<char *>(INFO_TITLE), NULL},
                        {ITEM_ACTIVE, 108, const_cast<char *>(EXIT_TITLE), NULL},
                        {0, 0, NULL, NULL}};

    OpenMenu(mainMenu, MENU_POSITION, m_menuRect.x, m_menuRect.y, p_handler);

    return 0;
}
