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
  std::string by; 
  int time; //unix timestamp
  std::string text;
  int parent = 0;
  std::vector<int> kids;
  int score;
  std::string title;
  int descendants = 0;
};

struct hnUser
{
  std::string id;
  std::string about;
  int created; //unix timestamp
  int karma;
  std::vector<int> submitted;
};
#endif