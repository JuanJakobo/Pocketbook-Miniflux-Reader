//------------------------------------------------------------------
// item.h
//
// Author:           JuanJakobo
// Date:             31.05.2021
// Description:      
//-------------------------------------------------------------------

#ifndef ITEM
#define ITEM

#include <string>
#include <vector>
struct hnItem
{
  int id;
  bool deleted;
  std::string by; // The username of the item's author
  //time;
  std::string text;
  int parent = 0;
  std::vector<int> kids;
  int score;
  std::string title;
  int descendants = 0;
};
#endif