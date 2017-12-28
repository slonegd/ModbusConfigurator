#pragma once

#include <ncursesw/ncurses.h>
#include <string>
#include <algorithm>
#include <wchar.h>
#include "ncursesExt.h"

class Iwidget
{
public:
   // отрисовка текста
   virtual void draw() = 0;
   // отрисовка текущего значения на фоне color
   virtual void drawCurrent(uint8_t color) = 0;
   // обработчик нажатия на enter
   virtual void enterHandler() = 0;
   // обработчик нажатия на вверх
   virtual void upHandler() = 0;
   // обработчик нажатия на вниз
   virtual void downHandler() = 0;
   // для менюшек, вошли ли в них
   virtual bool isEnter() = 0;
   // обработчик задания с клавиатуры
   virtual void enterValHandler (wchar_t ch) = 0;
   // для кнопки нажата ли
   virtual bool isPush() = 0;
   // 
   // virtual void drawBox() = 0;
   virtual ~Iwidget() {};

};


// интерфейс для таблицы
class Itabular
{
public:
   // название параметра
   virtual std::wstring strName() = 0;
   // текущее значение параметра
   virtual std::wstring strValue() = 0;
};