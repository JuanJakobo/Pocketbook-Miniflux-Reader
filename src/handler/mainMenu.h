#pragma once
//------------------------------------------------------------------
// mainMenu.h
//
// Author:           JuanJakobo
// Date:             14.06.2020
// Description:      Handles the menubar and the menu
//-------------------------------------------------------------------
#include "inkview.h"

#include <string>

class MainMenu
{
public:
  /**
   * Defines fonds, sets global Event Handler and starts new content
   *
   * @param name name of the application
   */
  MainMenu(const std::string &p_name);

  ~MainMenu() = default;

  const irect &getContentRect() const
  {
      return m_contentRect;
  };
  const irect &getMenuButtonRect() const
  {
      return m_menuButtonRect;
  };

  /**
   * Draws the menu panel
   */
  void draw() const;

  /**
   * Shows the menu on the screen, lets the user choose menu options and then
   * redirects the handler to the caller
   *
   * @param true if main view is shown
   * @param handler handles the clicks on the menu
   */
  int open(bool p_mainView, const iv_menuhandler &p_handler) const;

private:
  const std::string m_name;
  irect m_menuRect;
  irect m_contentRect;
  irect m_menuButtonRect;
};
