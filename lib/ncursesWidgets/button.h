#pragma once

#include "pseudograph.h"
#include "interfaces.h"
#include "ncursesExt.h"

using namespace NCURSES;

// кнопка
// конструктору необходим массив с 2 строками
// 0 - кнопка не нажата
// 1 - кнопка нажата
class Button : public Iwidget
{
public:
   std::wstring name[2];
   const int posY;
	const int posX;
   int weight;


   // конструктор кнопки с двумя состояниями
   Button (const std::wstring (&name)[2], int posY, int posX)
      : name(name), posY (posY), posX (posX), push (false)
   {
      weight = 4 + std::max (this->name[0].size(), this->name[1].size() );
   }


   // конструктор кнопки с одним состоянием
   Button (const std::wstring name, int posY, int posX)
      : posY (posY), posX (posX), push (false)
   {
      this->name[0] = name;
      this->name[1] = name;
      weight = 4 + name.size();
   }


   void draw()
   {
      mvaddwstr (posY, posX, PSEUDO::upBorder (weight).c_str() );
      std::wstring tmpstr = L"";
      tmpstr += PSEUDO::vertical();
      mvaddwstr (posY + 1, posX, tmpstr.c_str() );
      mvaddwstr (posY + 1, posX + weight - 1, tmpstr.c_str() );
      mvaddwstr (posY + 2, posX, PSEUDO::downBorder (weight).c_str() );
      drawCurrent(black);
   }


   void drawCurrent(uint8_t color_)
   {
      std::wstring tmp = push ? name[1] : name[0];
      tmp = NCURSES::addSpaces( tmp, weight - 2 );
      NCURSES::mvwprintColor ( posY + 1, posX + 1,
                               tmp,
                               color_ );
   }


   void enterHandler()
   {
      push = !push;
      drawCurrent(color::green);
   }


   void unPush()
   {
      push = false;
      drawCurrent(color::green);
   }


   bool isPush() { return push; }


   void upHandler() { }
   void downHandler() { }
   void enterValHandler (wchar_t ch) { }
   bool isEnter() { return false; }
   void drawBox() { };

private:
   bool push;
};