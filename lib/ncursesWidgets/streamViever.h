#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <pseudograph.h>

class ModbusStreamViever
{
public:
   int posX;
   string data[40];
   int currentString;
   int currentY;
   int currentX;
   int height, weight;

   ModbusStreamViever(int posX)
      : posX(posX), currentString(0), currentY(0), currentX(posX) 
   {
      currentX++;
   }


   void draw()
   {
      getmaxyx(stdscr, height, weight);
      weight -= posX + 1;
      weight -= weight % 3;
      wstring tmpstr = L"";
      tmpstr += PSEUDO::vertical();
      for (int i = 0; i < height; i++)
         mvaddwstr (i, posX, tmpstr.c_str());
   }


   void addData (uint8_t* buf, int qty, int color_)
   {
      stringstream tmp;
      string tmpstr;
      tmp.fill( '0' ); 
      for (int i = 0; i < qty; ++i)
         tmp << uppercase << setw(2) << hex << (uint)buf[i] << ' ';
      tmpstr = tmp.str();
      while (tmpstr.size() > weight) {
         mvprintColor ( currentY, posX + 1, tmpstr.substr(0, weight), color_ );
         tmpstr = tmpstr.erase(0, weight);
         endLine();
      }
      if (tmpstr.size() > 0)
         mvprintColor ( currentY, posX + 1, tmpstr, color_ );
      endLine();
   }


   void addString (string str, int color_)
   {
      mvprintColor ( currentY, posX + 1, str, color_ );
      endLine();
   }


   void endLine()
   {
      currentY++;
      if (currentY == height)
         currentY = 0;
      wstring tmpstr = L"";
      for (int i = 1; i < weight; i++)
         tmpstr += ' ';
      mvaddwstr (currentY, posX + 1, tmpstr.c_str());
   }
};

