
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <ncursesMenu.h>
#include <MBmaster.h>
#include <string>

using namespace std;
using namespace NCURSES;

const string portFile = "/dev/ttyUSB0";

int main() 
{
   setlocale (LC_ALL, "");

   wstring boudrate[] = {
      L"boudrate",
      L"9600",
      //L"14400",
      L"19200",
      //L"28800",
      L"38400",
      L"57600",
      //L"76800",
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

   system("resize -s 40 150");
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

   auto stream = ModbusStreamViever (addressValue.weight + addressValue.posX + 1, 3);
   auto modbus = MBmaster(stream, portFile);
   using MBstate = MBmaster::State;
   using MBfunc = MBmaster::MBfunc;

   int current = 0;
   menu[current]->drawCurrent(color::green);

   bool work = true;

   enum StateWork {
      startMenu,
      openPort,
      tryConnect
   } state = startMenu;

   while (work) {
      switch (state) {

      case startMenu:
         //int ch;
         wchar_t ch;
         ch = getch();
         switch (ch) {
            case KEY_UP:   menu[current]->upHandler();    break;
            case KEY_DOWN: menu[current]->downHandler();  break;
            case '\n':     menu[current]->enterHandler(); break;
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
            case KEY_END: work = false; break; 
            default: menu[current]->enterValHandler(ch);  break;

         } // switch (ch)

         if ( connectBut.isEnter() ) {
            connectBut.hide();
            state = openPort;
         }
         break;

      case openPort:
         using BR = MBmaster::Boudrate;
         BR br;
         br = boudrateMenu.curChoice == 1 ? BR::br9600  :
              boudrateMenu.curChoice == 2 ? BR::br19200 :
              boudrateMenu.curChoice == 3 ? BR::br38400 :
              boudrateMenu.curChoice == 4 ? BR::br57600 :
                                            BR::br115200;
         using PR = MBmaster::Parity;
         PR pr;
         pr = parityMenu.curChoice == 1 ? PR::none :
              parityMenu.curChoice == 2 ? PR::even :
                                          PR::odd;
         if ( modbus.port.open_ (br, pr, stoBitsMenu.curChoice, 500) ) {
            stream.addString ("Open " + portFile, NCURSES::color::tRed);
            state = tryConnect;
            modbus.state = MBmaster::State::pack;
         } else {
            stream.addString ("Can`t open " + portFile, NCURSES::color::tRed);
            state = startMenu;
            connectBut.show();
         }

         break;

      case tryConnect:
         int devAdr, regAdr;
         devAdr = addressValue.value;
         regAdr = 4;
         modbus.tx_rx (MBfunc::Read_Registers_03, devAdr, regAdr, 19);
         if ( modbus.state == MBstate::doneNoErr ) {
            state = startMenu;
            connectBut.show();
         } else if ( modbus.isError() ) {
            state = startMenu;
            connectBut.show();
         }
         // work = false;
         break;
      } // switch (state)

   } // while (1)



   getch();
   endwin();       // Выход из curses-режима. Обязательная команда.

   return 0; 
}