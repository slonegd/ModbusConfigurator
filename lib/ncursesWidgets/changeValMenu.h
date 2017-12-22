#pragma once

#include <pseudograph.h>
#include <Iwidget.h>

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
   const int posY;
	const int posX;
   int weight;
   bool enter;
   bool setValue;

   ChangeValMenu (const std::wstring& name, int posY, int posX,
                  int value, int valMin, int valMax)
      : value (value), valMin (valMin), valMax (valMax), preValue (0),
        posY (posY), posX (posX),
        enter (false),  setValue (false)
   {
      weight = std::max ( name.size() + 4, 2 * to_wstring(valMax).size() + 4);

      wstring tmpstr;
      for (int i = 0; i < weight - 2; ++i)
         tmpstr += PSEUDO::horisontal();
      mvaddwstr (posY, posX, (PSEUDO::upLeft() + tmpstr + PSEUDO::upRight()).c_str() );

      this->name = L"";
      this->name += PSEUDO::vertical();
      for (int i = 0; i < weight - (int)name.size() - 3; ++i)
         this->name += L' ';
      this->name += name + L' ';
      this->name += PSEUDO::vertical();
      drawName();
      drawCurrent(color::black);
      mvaddwstr (posY + 3, posX, (PSEUDO::downLeft() + tmpstr + PSEUDO::downRight()).c_str() );
      tmpstr = L"";
      tmpstr += PSEUDO::vertical();
      mvaddwstr (posY + 2, posX, tmpstr.c_str() );
      mvaddwstr (posY + 2, posX + weight - 1, tmpstr.c_str() );
   }


   void drawName()
   {
      mvaddwstr (posY + 1, posX, name.c_str());
   }


   void drawCurrent(uint8_t color_)
   {
      std::wstring valstr = to_wstring(value);
      std::wstring tmpstr = L" ";
      for (uint8_t i = 0; i < weight - valstr.size() - 4; ++i)
         tmpstr += L' ';
      tmpstr += valstr + L" ";
      mvwprintColor (posY + 2, posX + 1, tmpstr, color_);
   }


   void enterHandler()
   {
      enter = !enter;
      if (enter) {
         drawCurrent(color::blue);
         move (posY + 2, posX + 2);
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
};