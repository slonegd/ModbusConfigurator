#pragma once

#include <pseudograph.h>
#include <Iwidget.h>

using namespace NCURSES;
using namespace std;

// меню выбора из выпадающего вниз меню
// конструктору необходима ссылка на массив строк
// первая страка названия меню, вторая - варианты
// по умолчанию выбран первый вариант
// так же необходимо задать позицию, откуда отрисовывать
class PullDownMenu : public Iwidget
{
public:
	int posY;
	int posX;
   int weight;
   std::wstring* name;
   uint8_t qty;
   int curChoice;
   bool enter;


   // конструктор с названием и со значением в одном поле (мини)
   template <uint8_t n>
	PullDownMenu (std::wstring (&name)[n], int posY, int posX) 
      : posY (posY), posX (posX), weight(0), name (name), qty(n),
        curChoice(1), enter(false)
	{ 
      for (uint8_t i = 0; i < qty; ++i)
         weight = ( (int)name[i].size() > weight) ? name[i].size() : weight;
      const int addSpace = 4; // рамки и пробелы
      weight += addSpace; 
      wstring tmpstr;
      for (int i = 0; i < qty; ++i) {
         int tmp;
         tmp = weight - addSpace - name[i].size();
         tmpstr = L' ';
         for (int j = 0; j < tmp; ++j)
            tmpstr = tmpstr + L' ';
         name[i] = tmpstr + name[i] + L' ';
      }
   }

   // конструктор с заданной шириной полей названия и значения
   template <uint8_t n>
	PullDownMenu (std::wstring (&name)[n], int posY, int posX, int nameWeight, int valWeight) 
      : posY (posY), posX (posX), weight(0), name (name), qty(n),
        curChoice(1), enter(false)
   {

   }

   void draw()
   {
      wstring tmpstr;
      // tmpstr = PSEUDO::upLeft();
      for (int i = 0; i < weight - 2; ++i)
         tmpstr += PSEUDO::horisontal();
      // tmpstr += PSEUDO::upRight();
      mvaddwstr (posY, posX, (PSEUDO::upLeft() + tmpstr + PSEUDO::upRight()).c_str() );
      mvaddwstr (posY + 3, posX, (PSEUDO::downLeft() + tmpstr + PSEUDO::downRight()).c_str() );
      tmpstr = PSEUDO::vertical();
      mvaddwstr (posY + 2, posX, tmpstr.c_str() );
      mvaddwstr (posY + 2, posX + weight - 1, tmpstr.c_str() );
      
      drawName();
      drawCurrent(color::black);
   }


   void drawName()
   {
      mvaddwstr (posY + 1, posX, (PSEUDO::vertical() + name[0] + PSEUDO::vertical()).c_str());
   }


   void drawCurrent(uint8_t color)
   {
      mvwprintColor (posY + 2, posX + 1, name[curChoice], color);
   }


   void drawAll()
   {
      uint8_t color;
      wstring tmpstr = L"";
      for (int i = 0; i < weight - 2; ++i)
         tmpstr += PSEUDO::horisontal();
      mvaddwstr (posY, posX, (PSEUDO::upLeft() + tmpstr + PSEUDO::upRight()).c_str() );
      wstring vert = L"";
      vert += PSEUDO::vertical();
      for (int i = 0; i < qty; ++i) {
         color = (i != curChoice) ? 4 : 1;
         mvaddwstr (posY + 1 + i, posX, vert.c_str() );
         mvwprintColor (posY + 1 + i, posX + 1, name[i], color);
         mvaddwstr (posY + 1 + i, posX + weight - 1, vert.c_str() );
      }
      mvaddwstr (posY + qty + 1, posX, (PSEUDO::downLeft() + tmpstr + PSEUDO::downRight()).c_str() );
   }


   bool isEnter()
   {
      return enter;
   }


   void enterHandler()
   {
      enter = !enter;
      if (enter) {
         scr_dump("dump.out");
         drawAll();
      } else {
         scr_restore("dump.out");
         drawCurrent(1);
      }
   }


   void downHandler()
   {
      if (curChoice < qty - 1 && enter ) {
         mvwprintColor (posY + curChoice + 1, posX + 1, name[curChoice], 4);
         curChoice++;
         mvwprintColor (posY + curChoice + 1, posX + 1, name[curChoice], 1);
      }
   }


   void upHandler()
   {
      if (curChoice > 1 && enter ) {
         mvwprintColor (posY + curChoice + 1, posX + 1, name[curChoice], 4);
         curChoice--;
         mvwprintColor (posY + curChoice + 1, posX + 1, name[curChoice], 1);
      }
   }


   void enterValHandler (wchar_t ch) { }
   bool isPush() { return false; }

};



