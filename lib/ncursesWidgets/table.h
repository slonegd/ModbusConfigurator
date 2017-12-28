#pragma once

#include "pseudograph.h"
#include "interfaces.h"


using namespace NCURSES;
using namespace PSEUDO;
using namespace std;

// идея
// таблица праметров
// количество параметров заранее не известно
// типы параметров тоже
// должен быть метод добавления параметра любого типа
// соответсвенно таблица должна хранить эти параметры
// любой тип не пойдёт, нужен интерфейс

class Table
{
public:
   int y;
   int x;
   int nameWeight;
   int valWeight;
   int height;

   Table(int y, int x ,int nameWeight, int valWeight, int lineQty)
      : y(y), x(x), nameWeight(nameWeight), valWeight(valWeight),
        height(2*lineQty + 1), lineQty(lineQty)
   { }


   void draw()
   {
      // начальный, между границами, внутренняя граница, конечный символы
      auto oneLine = [&](auto begin, auto within, auto inner, auto end)
      {
         wstring res;// = L"";
         res = begin;
         for (int i = 0; i < nameWeight; i++)
            res += within;
         res += inner;
         for (int i = 0; i < valWeight; i++)
            res += within;
         return res += end;
      };

      wstring upBorder    = oneLine ( upLeft(),      horisontal(), upWithTap(),   upRight() );
      wstring innerVoid   = oneLine ( vertical(),    L' ',         vertical(),    vertical() );
      wstring innerBorder = oneLine ( leftWithTap(), horisontal(), crossroad(),   rightWithTap() );
      wstring downBorder  = oneLine ( downLeft(),    horisontal(), downWithTap(), downRight() );

      mvaddwstr (y, x, upBorder.c_str() );
      for (int i = 0; i < lineQty; i++) {
         mvaddwstr (y + 2*i + 1, x, innerVoid.c_str() );
         if (i == lineQty - 1) 
            break;
         mvaddwstr (y + 2*i + 2, x, innerBorder.c_str() );
      }
      mvaddwstr (y + 2*lineQty, x, downBorder.c_str() );
   }



private:
   //int qty;
   int lineQty;


   void addSpaces (wstring& ws)
   {
      wstring tmp = L"";
      for (int i = 0; i < (int)(valWeight - ws.size() - 5); i++)
         tmp += L' ';
      ws = tmp + ws;
   }


};