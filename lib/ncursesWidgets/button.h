#pragma once

#include <pseudograph.h>
#include <Iwidget.h>

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


   Button (const std::wstring (&name)[2], int posY, int posX)
      : posY (posY), posX (posX), push (false)
   {
      this->name[0] = L" " + name[0] + L" ";
      weight = this->name[0].size();
      this->name[1] = L" " + name[1] + L" ";
      weight = 2 + max (this->name[0].size(), this->name[1].size() );

   }


   void draw()
   {
      mvaddwstr (posY, posX, PSEUDO::upBorder (weight).c_str() );
      wstring tmpstr = L"";
      tmpstr += PSEUDO::vertical();
      mvaddwstr (posY + 1, posX, tmpstr.c_str() );
      mvaddwstr (posY + 1, posX + weight - 1, tmpstr.c_str() );
      mvaddwstr (posY + 2, posX, PSEUDO::downBorder (weight).c_str() );
      drawName();
   }


   void drawCurrent(uint8_t color_)
   {
      mvwprintColor ( posY + 1, posX + 1,
                      push ? name[1] : name[0],
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


   bool isPush()
   {
      return push;
   }


   void upHandler() { }
   void downHandler() { }
   void enterValHandler (wchar_t ch) { }
   bool isEnter() { return false; }

private:
   bool push;

   void drawName()
   {
      mvwprintColor (posY + 1, posX + 1, name[0], color::black);
   }
};