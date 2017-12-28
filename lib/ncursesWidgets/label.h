#pragma once

#include "interfaces.h"

class Label
{
public:
   Label (int posY, int posX, int weight)
      : posY(posY), posX(posX), weight(weight)
   { }

   // первая строка
   void setLabel1 (const wstring & str)
   {
      ws[0] = center(str, weight - 2);

   }


   // вторая строка
   void setLabel2 (const wstring & str)
   {
      ws[1] = center(str, weight - 2);
   }


   void draw()
   {
      mvaddwstr ( posY + 1, posX + 1, ws[0].c_str() );
      mvaddwstr ( posY + 2, posX + 1, ws[1].c_str() );
      wstring tmp;
      tmp = upLeft();
      tmp += line (weight - 2, horisontal());
      tmp += upRight();
      mvaddwstr( posY, posX, tmp.c_str() );
      tmp.replace (0, 1, 1, downLeft());
      tmp.replace (weight - 1, weight, 1, downRight());
      mvaddwstr( posY + 3, posX, tmp.c_str() );
      tmp = vertical();
      for (int i = 0; i < 2; i++) {
         mvaddwstr(posY + 1 + i, 0, tmp.c_str());
         mvaddwstr(posY + 1 + i, weight - 1, tmp.c_str());
      }
   }


   void hide()
   {
      
   }



private:
   int posY;
   int posX;
   int weight;
   wstring ws[2];


};