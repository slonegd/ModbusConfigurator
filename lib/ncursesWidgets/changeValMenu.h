#pragma once

#include "pseudograph.h"
#include "interfaces.h"
#include "table.h"

using namespace NCURSES;
using namespace std;

// меню задания параметра
class ChangeValMenu : public Iwidget
{
public:
   std::wstring name;
   int value;
   int valMin;
   int valMax;
   int preValue;
   int posY;
	int posX;
   int nameY;
   int nameX;
   int valY;
   int valX;
   int weight;
   int valWeight;
   bool enter;
   bool setValue;


   // конструктор с названием и значением в одном боксе
   ChangeValMenu (const std::wstring& name, int posY, int posX,
                  int value, int valMin, int valMax)
      : value (value), valMin (valMin), valMax (valMax), preValue (0),
        posY (posY), posX (posX), 
        nameY(posY+1), nameX(posX+1),
        valY(nameY+1), valX(nameX),
        enter (false),  setValue (false)
   {
      this->name = name;
      valWeight = std::max ( name.size() + 2, 2 * to_wstring(valMax).size() + 1 + 2);
      weight = valWeight + 2;
   }


   // конструктор с занесением в таблицу
   ChangeValMenu (const std::wstring& name, const Table& table, int line,
               int value, int valMin, int valMax)
      : value (value), valMin (valMin), valMax (valMax), preValue (0),
        posY (table.y + 2*line), posX (table.x + table.nameWeight + 1), 
        nameY(posY+1), nameX(table.x+1), 
        valY(nameY), valX(posX+1),
        weight(table.valWeight + 2),
        valWeight(table.valWeight),
        enter (false),  setValue (false)
   {
      this->name = name;
   }


   void draw()
   {
      if (nameY == valY) {
         mvaddwstr (nameY, nameX + 1, name.c_str() );
      } else {
         drawBox(2);
         mvaddwstr (nameY, nameX, addSpaces (name, weight-2).c_str() );
      }
      drawCurrent(color::black);
   }


   void drawCurrent(uint8_t color_)
   {
      std::wstring valstr = to_wstring(value);
      valstr = addSpaces(valstr, valWeight);
      mvwprintColor (valY, valX, valstr, color_);
   }


   void enterHandler()
   {
      enter = !enter;
      if (enter) {
         drawCurrent(color::blue);
         move (valY, valX + 1);
         echo();
         preValue = 0;
         setValue = false;
      } else {
         if (setValue && preValue >= valMin && preValue <= valMax)
            value = preValue;
         drawCurrent(color::green);
         noecho();
      }
   }


   void upHandler()
   {
      int tmp = value;
      if (enter) {
         tmp++;
         if (value != tmp && tmp <= valMax) {
            value = tmp;
            drawCurrent(color::blue);
            move (posY + 1, posX + 1);
            echo();
            setValue = false;
            preValue = 0;
         }
      }
   }


   void downHandler()
   {
      int tmp = value;
      if (enter) {
         tmp--;
         if (value != tmp && tmp >= valMin) {
            value = tmp;
            drawCurrent(color::blue);
            move (posY + 1, posX + 1);
            echo();
            setValue = false;
            preValue = 0;
         }
      }
   }


   bool isEnter()
   {
      return enter;
   }


   void enterValHandler (wchar_t ch)
   {
      if (enter) {
         setValue = true;
         if (ch >= L'0' && ch <= L'9')
            preValue = preValue * 10 + (ch - L'0');
      }
   }

   void drawAll() { }
   bool isPush() { return false; }



private:
   void drawBox (int lines)
   {
      //int x = posX + valWeight - valMaxWeight;
      wstring tmp = L"";
      tmp += upLeft();
      tmp += line (valWeight, horisontal());
      tmp += upRight();
      mvaddwstr (posY, posX, tmp.c_str() );
      tmp.replace(0, 1, 1, downLeft());
      tmp.replace(valWeight + 1, valWeight + 2, 1, downRight());
      mvaddwstr (posY + lines + 1, posX, tmp.c_str() );
      tmp = vertical();
      for (int i = 1; i <= lines; i++) {
         mvaddwstr (posY + i, posX, tmp.c_str() );
         mvaddwstr (posY + i, posX + weight - 1, tmp.c_str() );
      }
   }


};