#pragma once

#include "pseudograph.h"
#include "interfaces.h"
#include "table.h"

using namespace NCURSES;
using namespace PSEUDO;
using namespace std;

// меню выбора из выпадающего вниз меню
// конструктору необходима ссылка на массив строк
// первая страка названия меню, вторая - варианты
// по умолчанию выбран первый вариант
// так же необходимо задать позицию, откуда отрисовывать
class PullDownMenu : public Iwidget//, public Itabular
{
public:
	int posY;
	int posX;
   int weight;
//   int height;
   int nameWeight;
   int valWeight;
   std::wstring* name;
   uint8_t qty;
   int curChoice;
   bool enter;
   int nameY;
   int nameX;
   int valY;
   int valX;
   int valMaxWeight;


   // конструктор с названием и со значением в одном поле (мини)
   template <uint8_t n>
	PullDownMenu (std::wstring (&name)[n], int posY, int posX) 
      : posY (posY), posX (posX),
        name (name), qty(n),
        curChoice(1), enter(false),
        nameY(posY + 1), nameX(posX + 1), valY(posY + 2), valX(posX + 1)
	{ 
      for (uint8_t i = 0; i < qty; ++i)
         weight = ( (int)name[i].size() > weight) ? name[i].size() : weight;
      weight += 2; // пробелы
      nameWeight = valWeight = valMaxWeight = weight;
      weight += 2; // рамки
   }


   // конструктор с внесением в таблицу
   template <uint8_t n>
   PullDownMenu (std::wstring (&name)[n], const Table& table, int line)
      : posY (table.y + line * 2), posX (table.x + table.nameWeight + 1),
        nameWeight(table.nameWeight), valWeight(table.valWeight),
        name(name), qty(n),
        curChoice(1), enter(false),
        nameY(posY + 1), nameX(table.x + 1), valY(nameY), valX(posX + 1)
   {
      for (uint8_t i = 1; i < qty; ++i)
         valMaxWeight = ( (int)name[i].size() > valMaxWeight ) 
                        ? name[i].size() : valMaxWeight;
      valMaxWeight += 2;
   }


   void draw()
   {
      if (nameY == valY)
         mvaddwstr (nameY, nameX + 1, name[0].c_str() );
      else
         mvaddwstr (nameY, nameX, addSpaces (name[0], nameWeight).c_str() );
      mvaddwstr (valY, valX, addSpaces (name[curChoice], valWeight).c_str() );
      if (nameX == valX)
         drawBox(2);
   }


   void drawCurrent(uint8_t color)
   {
      wstring tmp = addSpaces(name[curChoice], valWeight);
      mvwprintColor (valY, valX, tmp.c_str(), color);
   }


   bool isEnter() { return enter; }


   void enterHandler()
   {
      enter = !enter;
      if (enter) {
         scr_dump("./screens/dump.out");
         drawAll();
      } else {
         scr_restore("./screens/dump.out");
         drawCurrent(1);
      }
   }


   void downHandler()
   {
      if (curChoice < qty - 1 && enter ) {
         curChoice++;
         drawAll();
      }
   }


   void upHandler()
   {
      if (curChoice > 1 && enter ) {
         curChoice--;
         drawAll();
      }
   }


   void enterValHandler (wchar_t ch) { }
   bool isPush() { return false; }



private:
   void drawBox (int lines)
   {
      int x = posX + valWeight - valMaxWeight;
      wstring tmp = L"";
      tmp += upLeft();
      tmp += line (valMaxWeight, horisontal());
      tmp += upRight();
      mvaddwstr (posY, x, tmp.c_str() );
      tmp.replace(0, 1, 1, downLeft());
      tmp.replace(valMaxWeight + 1, valMaxWeight + 2, 1, downRight());
      mvaddwstr (posY + lines + 1, x, tmp.c_str() );
      tmp = vertical();
      for (int i = 1; i <= lines; i++) {
         mvaddwstr (posY + i, x, tmp.c_str() );
         mvaddwstr (posY + i, x + valMaxWeight + 1, tmp.c_str() );
      }
   }


   void drawAll()
   {
      if (valY == nameY)
         drawBox (qty - 1);
      else
         drawBox (qty);
      wstring tmp;
      color col;
      for (int i = 1; i < qty; i++) {
         tmp = addSpaces(name[i], valMaxWeight);
         col = (i == curChoice) ? green : black;
         mvwprintColor (valY  + i - 1, valX + valWeight - valMaxWeight, tmp.c_str(), col);
      }
   }
};



