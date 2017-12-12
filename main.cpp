//#define NCURSES_CH_T wchar_t

#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <ncursesw/ncurses.h>
#include <pullDownMenu.h>
#include <string>
#include <tuple>

using namespace std;




enum color {
   green = 1, red, blue, black
};


//std::array<char*, 5> subMenu;



void ColorPairInit()
{
   start_color();
   init_pair (green, COLOR_BLACK, COLOR_GREEN);
   init_pair (red,   COLOR_BLACK, COLOR_RED);
   init_pair (blue,  COLOR_BLACK, COLOR_BLUE);
   init_pair (black, COLOR_WHITE, COLOR_BLACK);
}

int main() 
{

   setlocale (LC_ALL, "");

   wstring boudrate[] = {
      L"boudrate",
      L"9600",
      L"14400",
      L"28800",
      L"115200",
      L"русский",
      L"english"
   };
   wstring parity[] = {
      L"проверка",
      L"отсутсвует",
      L"на чётность",
      L"на нечётность"
   };
   wstring stopBits[] = {
      L"стоп биты",
      L"1",
      L"2"
   };

   wstring address[] = {
      L"стоп биты",
      L"1",
      L"2"
   };

   initscr();      // Переход в curses-режим
   curs_set (0);   // невидимый курсор
   noecho();
   ColorPairInit();


/*   PullDownMenu menu [3] = {
      PullDownMenu(boudrate, 0, 0),
      PullDownMenu(parity,   0, menu[0].weight),
      PullDownMenu(stopBits, 0, menu[1].weight + menu[1].posX)
   };*/

   auto boudrateMenu = PullDownMenu(boudrate, 0, 0);
   auto parityMenu   = PullDownMenu(parity,   0, boudrateMenu.weight);
   auto stoBitsMenu  = PullDownMenu(stopBits, 0, parityMenu.weight + parityMenu.posX);

   IkeyboardProcessing* menu[3] = {
      &boudrateMenu,
      &parityMenu,
      &stoBitsMenu      
   };

/*   auto menu_ = make_tuple (
      PullDownMenu(boudrate, 0, 0),
      PullDownMenu(parity,   0, menu[0].weight),
      PullDownMenu(stopBits, 0, menu[1].weight + menu[1].posX)
   );
   */


   for (uint8_t i = 0; i < 3; ++i) {
      menu[i]->drawName();
      menu[i]->drawCurrent(4);
   }

   menu[0]->drawCurrent(1);


   
   keypad (stdscr, true);

   bool menuOn = true;
   bool enter = false;
   int current = 0;
   while (menuOn) {
      int ch;
      ch = getch();
      switch (ch) {
         case KEY_UP:   menu[current]->upHandler();    break;
         case KEY_DOWN: menu[current]->downHandler();  break;
         case '\n':     menu[current]->enterHandler(); break;
         case KEY_LEFT:
            if ( !menu[current]->isEnter() && current > 0 ) {
               menu[current]->drawCurrent(4);
               current--;
               menu[current]->drawCurrent(1);
            }
            break;
         case KEY_RIGHT:
            if ( !menu[current]->isEnter() && current < 2 ) {
               menu[current]->drawCurrent(4);
               current++;
               menu[current]->drawCurrent(1);
            }
            break;
         case KEY_BACKSPACE: menuOn = false;    break;
      } // switch (ch)

   } // while (menuOn)

   getch();
   endwin();       // Выход из curses-режима. Обязательная команда.

   return 0; 
}