
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
//#include <ncursesw/ncurses.h>
#include <ncursesMenu.h>
#include <MBmaster.h>
#include <string>

using namespace std;
using namespace NCURSES;

int main() 
{
   setlocale (LC_ALL, "");

   wstring boudrate[] = {
      L"boudrate",
      L"9600",
      L"14400",
      L"19200",
      L"28800",
      L"38400",
      L"57600",
      L"76800",
      L"115200"
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

   system("resize -s 30 120");
   initscr();      // Переход в curses-режим
   curs_set (0);   // невидимый курсор
   noecho();       // не отображать вводимые символы
   keypad (stdscr, true);
   ColorPairInit();

   auto boudrateMenu = PullDownMenu(boudrate,  0, 0);
   auto parityMenu   = PullDownMenu(parity,    0, boudrateMenu.weight);
   auto stoBitsMenu  = PullDownMenu(stopBits,  0, parityMenu.weight + parityMenu.posX);
   auto addressValue = ChangeValMenu(L"адрес", 0, stoBitsMenu.weight + stoBitsMenu.posX,
                                     1, 1, 255
                       );
   auto connectBut   = Button (L"подключиться", 0, addressValue.weight + addressValue.posX);

   uint8_t menuQty = 5;

   IkeyboardProcessing* menu[menuQty] = {
      &boudrateMenu,
      &parityMenu,
      &stoBitsMenu,
      &addressValue,
      &connectBut
   };

   int current = 0;
   menu[current]->drawCurrent(color::green);

   bool work = true;

   enum State {
      startMenu,
      tryConnect
   } state = startMenu;

   while (work) {
      switch (state) {

      case startMenu:
         //int ch;
         wchar_t ch;
         ch = getch();
         switch (ch) {
         case KEY_UP:   
            menu[current]->upHandler();    
            break;
         case KEY_DOWN: 
            menu[current]->downHandler();  
            break;
         case '\n':     
            menu[current]->enterHandler(); 
            break;
         case KEY_LEFT:
            if ( !menu[current]->isEnter() && current > 0 ) {
               menu[current]->drawCurrent(color::black);
               current--;
               menu[current]->drawCurrent(color::green);
            }
            break;
         case KEY_RIGHT:
            if ( !menu[current]->isEnter() && current < menuQty - 1 ) {
               menu[current]->drawCurrent(color::black);
               current++;
               menu[current]->drawCurrent(color::green);
            }
            break;
         default: 
            menu[current]->enterValHandler(ch); 
            break;
         } // switch (ch)

         if ( connectBut.isEnter() ) {
            connectBut.hide();
            state = tryConnect;
         }
         break;

      case tryConnect:

         auto stream = ModbusStreamViever (addressValue.weight + addressValue.posX + 1);
         auto modbus = MBmaster(stream);
         using State = MBmaster::State;
         using MBfunc = MBmaster::MBfunc;
         int devAdr, regQty, regAdr;
         devAdr = 1;
         regAdr = 4;
         modbus.tx_rx (MBfunc::Read_Registers_03, devAdr, regAdr, 19);
         if (modbus.state == State::transmit)
            work = false;
         // work = false;
         break;
      } // switch (state)

   } // while (1)



   getch();
   endwin();       // Выход из curses-режима. Обязательная команда.

   return 0; 
}