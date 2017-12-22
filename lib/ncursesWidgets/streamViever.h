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

   ModbusStreamViever(int posX, int posY)
      : posX(posX), currentString(0), currentY(posY), currentX(posX) 
   {
      mvprintw (currentY, currentX, "|");
      currentX++;
   }

   void addData (uint8_t* buf, int qty, int color_)
   {
      stringstream tmp;
      tmp.fill( '0' ); 
      for (int i = 0; i < qty; ++i)
         tmp << uppercase << setw(2) << hex << (uint)buf[i] << ' ';
      data[currentString] += tmp.str();
      mvprintColor ( currentY, posX + 1, data[currentString], color_ );
   }
   void addString (string str, int color_)
   {
      mvprintColor ( currentY, posX + 1, str, color_ );
      endLine();
   }

   void endLine()
   {
      currentY++;
      currentString++;
      currentX = posX;
      mvprintw (currentY, currentX, "|");
      currentX++;
   }
   
};

