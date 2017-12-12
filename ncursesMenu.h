#pragma once

#include <ncurses.h>
#include <string>
#include <algorithm>
#include <wchar.h>
#include <iostream>
#include <sstream>
#include <iomanip>


namespace NCURSES {

   // рисует сроку по координатом с цветом
   void mvwprintColor (int y, int x, const std::wstring& str, int color)
   {
      attron (COLOR_PAIR(color));
      mvaddwstr (y, x, str.c_str());
      attroff (COLOR_PAIR(color));
   }

   void mvprintColor (int y, int x, const std::string& str, int color)
   {
      attron (COLOR_PAIR(color));
      mvaddstr (y, x, str.c_str());
      attroff (COLOR_PAIR(color));
   }

   enum color {
      green = 1, red, blue, black, tBlue, tGreen, tRed
   };
   void ColorPairInit()
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
}

using namespace NCURSES;
using namespace std;

class IkeyboardProcessing
{
public:
   virtual void drawCurrent(uint8_t color) = 0;
   virtual void enterHandler() = 0;
   virtual void upHandler() = 0;
   virtual void downHandler() = 0;
   virtual bool isEnter() = 0;
   virtual void enterValHandler (wchar_t ch) = 0;
   virtual ~IkeyboardProcessing() {};
};

class PullDownMenu : public IkeyboardProcessing
{
public:
	int posY;
	int posX;
   int weight;
   std::wstring* name;
   uint8_t qty;
   int curChoice;
   bool enter;

   template <uint8_t n>
	PullDownMenu (std::wstring (&name)[n], int posY, int posX) :
	 	name (name), qty(n), posY (posY), posX (posX), weight(0), curChoice(1), enter(false)
	{ 
      //this->name = name;
      for (uint8_t i = 0; i < qty; ++i)
         weight = (name[i].size() > weight) ? name[i].size() : weight;
      const int addSpace = 3; // пробел до и после
      weight += addSpace; 

      wstring tmpstr;
      for (int i = 0; i < qty; ++i) {
         int tmp;
         tmp = weight - addSpace - name[i].size();
         tmpstr = L" ";
         for (int j = 0; j < tmp; ++j)
            tmpstr = tmpstr + L' ';
         name[i] = tmpstr + name[i] + L" |";
      }
      
      drawName();
      drawCurrent(color::black);
   }

   ~PullDownMenu () {}

   void drawName()
   {
      mvaddwstr (posY, posX, name[0].c_str());
   }


   void drawCurrent(uint8_t color)
   {
      mvwprintColor (posY + 1, posX, name[curChoice], color);
      wstring tmpstr = L"";
      for (int i = 0; i < weight - 1; ++i)
         tmpstr += L'-';
      tmpstr += L'|';
      mvwprintColor (posY + 2, posX, tmpstr, 4);
   } 


   void drawAll()
   {
      uint8_t color;
      for (int i = 0; i < qty; ++i) {
         color = (i != curChoice) ? 4 : 1;
         mvwprintColor (posY + i, posX, name[i], color);
      }
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
         mvwprintColor (posY + curChoice, posX, name[curChoice], 4);
         curChoice++;
         mvwprintColor (posY + curChoice, posX, name[curChoice], 1);
      }
   }

   void upHandler()
   {
      if (curChoice > 1 && enter ) {
         mvwprintColor (posY + curChoice, posX, name[curChoice], 4);
         curChoice--;
         mvwprintColor (posY + curChoice, posX, name[curChoice], 1);
      }
   }

   void enterValHandler (wchar_t ch) { }

};

class ChangeValMenu : public IkeyboardProcessing
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
      : posY (posY), posX (posX),
        value (value), valMin (valMin), valMax (valMax), preValue (0),
        enter (false),  setValue (false)
   {
      weight = std::max ( name.size() + 3, 2 * to_wstring(valMax).size() + 4);
      this->name = L"";
      for (int i = 0; i < weight - name.size() - 2; ++i)
         this->name += L' ';
      this->name += name + L" |";
      drawName();
      drawCurrent(color::black);
   }
   void drawName()
   {
      mvaddwstr (posY, posX, name.c_str());
   }
   void drawCurrent(uint8_t color_)
   {
      std::wstring tmpstr1 = to_wstring(value);
      std::wstring tmpstr2 = L" ";
      for (uint8_t i = 0; i < weight - tmpstr1.size() - 3; ++i)
         tmpstr2 += L' ';
      tmpstr2 += tmpstr1 + L" |";
      mvwprintColor (posY + 1, posX, tmpstr2, color_);
      wstring tmpstr = L"";
      for (int i = 0; i < weight - 1; ++i)
         tmpstr += L'-';
      tmpstr += L'|';
      mvwprintColor (posY + 2, posX, tmpstr, color::black);
   } 
   void drawAll() { }
   void enterHandler()
   {
      enter = !enter;
      if (enter) {
         drawCurrent(color::blue);
         move (posY + 1, posX + 1);
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
};

class Button : public IkeyboardProcessing
{
public:
   std::wstring name;
   const int posY;
	const int posX;
   int weight;
   bool enter;

   Button (const std::wstring& name, int posY, int posX)
      : posY (posY), posX (posX), enter (false)
   {
      this->name = L"| " + name + L" |";
      weight = this->name.size();
      drawName();
   }

   void drawCurrent(uint8_t color_)
   {
      mvwprintColor (posY + 1, posX, name, color_);
   }
   void enterHandler()
   {
      enter = !enter;
   }
   void upHandler() {}
   void downHandler() {}
   bool isEnter()
   {
      return enter;
   }
   void enterValHandler (wchar_t ch) {}
   void hide()
   {
      wstring tmpstr = L"";
      for (int i = 0; i < weight; ++i)
         tmpstr += L' ';
      for (int i = 0; i < 3; ++i)
         mvwprintColor (posY + i, posX, tmpstr, color::black);
   }
private:
   void drawName()
   {
      wstring tmpstr = L"|";
      for (int i = 0; i < weight - 2; ++i)
         tmpstr += L'-';
      tmpstr += L'|';
      mvwprintColor (posY, posX, tmpstr, color::black);
      mvwprintColor (posY + 1, posX, name, color::black);
      mvwprintColor (posY + 2, posX, tmpstr, color::black);
   }
};


class ModbusStreamViever
{
public:
   int posX;
   string data[40];
   int currentString;
   int currentY;
   int currentX;

   ModbusStreamViever(int posX)
      : currentY(0), posX(posX), currentX(posX), currentString(0)
   {
      mvprintw (currentY, currentX, "|");
      currentX++;
   }

   void addData (char* buf, int qty, int color_)
   {
      stringstream tmp;
      tmp.fill( '0' ); 
      for (int i = 0; i < qty; ++i)
         tmp << uppercase << setw(2) << hex << (int)buf[i] << ' ';
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