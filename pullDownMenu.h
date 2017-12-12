#pragma once

#include <ncurses.h>
#include <string>
// #include <iostream>

class IkeyboardProcessing
{
public:
   virtual void drawName() = 0;
   virtual void drawCurrent(uint8_t color) = 0;
   virtual void drawAll() = 0;
   virtual void enterHandler() = 0;
   virtual void upHandler() = 0;
   virtual void downHandler() = 0;
   virtual bool isEnter() = 0;
   


/*   enum Dir { up, down };
   virtual void change (Dir dir);*/
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
   std::wstring* shadow;
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

      std::wstring tmpstr;;
      for (int i = 0; i < qty; ++i) {
         int tmp;
         tmp = weight - addSpace - name[i].size();
         tmpstr = L" ";
         for (int j = 0; j < tmp; ++j)
            tmpstr = tmpstr + L' ';
         name[i] = tmpstr + name[i] + L" |";
      }

      tmpstr = L"";
      for (uint8_t i = 0; i < weight; ++i)
         tmpstr = tmpstr + L' ';
      this->shadow = new std::wstring [qty];
      for (uint8_t i = 0; i < qty; ++i)
         shadow[i] = tmpstr;
   }

   ~PullDownMenu ()
   {
      delete [] shadow;
   }

   void drawName()
   {
      mvaddwstr (posY, posX, name[0].c_str());
   }


   void drawCurrent(uint8_t color)
   {
      mvprintColor (posY + 1, posX, name[curChoice], color);
      for (uint8_t i = 2; i < qty; ++i)
         mvprintColor (posY + i, posX, shadow[i], 4);
   } 


   void drawAll()
   {
      uint8_t color;
      for (uint8_t i = 0; i < qty; ++i) {
         color = (i != curChoice) ? 4 : 1;
         mvprintColor (posY + i, posX, name[i], color);
      }
   }

   bool isEnter()
   {
      return enter;
   }

   void enterHandler()
   {
      enter = !enter;
      enter ? drawAll() : drawCurrent(1);
   }

   void downHandler()
   {
      if (curChoice < qty - 1 && enter ) {
         mvprintColor (posY + curChoice, posX, name[curChoice], 4);
         curChoice++;
         mvprintColor (posY + curChoice, posX, name[curChoice], 1);
      }
   }

   void upHandler()
   {
      if (curChoice > 1 && enter ) {
         mvprintColor (posY + curChoice, posX, name[curChoice], 4);
         curChoice--;
         mvprintColor (posY + curChoice, posX, name[curChoice], 1);
      }
   }

   //enum Dir { up, down };
/*   void change (Dir dir)
   {
      // перевёрнутая логика верх это низ
      bool needGo = (dir == up && curChoice > 0) 
                 || (dir == down && curChoice < qty - 1);
      if (needGo) {
         mvprintColor (posY + curChoice, posX, name[curChoice], 4);
         curChoice = (dir == up) ? curChoice - 1 : curChoice + 1;
         mvprintColor (posY + curChoice, posX, name[curChoice], 1);
      }
   }*/
    


private:
   void mvprintColor (int y, int x, const std::wstring& str, int color)
   {
      attron (COLOR_PAIR(color));
      mvaddwstr (y, x, str.c_str());
      attroff (COLOR_PAIR(color));
   }
   
};