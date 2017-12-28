#pragma once

#include <ncursesw/ncurses.h>
#include <string>
#include <algorithm>
#include <wchar.h>

namespace NCURSES {

   // рисует сроку по координатом с цветом
   inline void mvwprintColor (int y, int x, const std::wstring& str, int color)
   {
      attron (COLOR_PAIR(color));
      mvaddwstr (y, x, str.c_str());
      attroff (COLOR_PAIR(color));
   }

   inline void mvprintColor (int y, int x, const std::string& str, int color)
   {
      attron (COLOR_PAIR(color));
      mvaddstr (y, x, str.c_str());
      attroff (COLOR_PAIR(color));
   }

   enum color {
      green = 1, red, blue, black, tBlue, tGreen, tRed
   };
   inline void ColorPairInit()
   {
      start_color();
      init_pair (green, COLOR_BLACK, COLOR_GREEN);
      init_pair (red,   COLOR_BLACK, COLOR_RED);
      init_pair (blue,  COLOR_BLACK, COLOR_CYAN);
      init_pair (black, COLOR_WHITE, COLOR_BLACK);
      init_pair (tBlue, COLOR_CYAN,  COLOR_BLACK);
      init_pair (tGreen,COLOR_GREEN, COLOR_BLACK);
      init_pair (tRed,  COLOR_RED,   COLOR_BLACK);
   }

   std::wstring addSpaces (const std::wstring& ws, int weight)
   {
      std::wstring res = L"";
      while ( res.size() < weight - ws.size() - 1 )
         res += L' ';
      return res + ws + L' ';
   };

   std::wstring center (const std::wstring& ws, int weight)
   {
      std::wstring res1 = L"";
      std::wstring res2 = L"";
      for (int i = 0; i < (weight - ws.size()) / 2; i++) 
         res1 += L' ';
      while ( (res1.size() + ws.size() + res2.size()) < weight )
         res2 += L' ';
      return res1 + ws + res2;
   };
}
