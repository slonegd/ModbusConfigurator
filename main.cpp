
#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <ncursesWidgets.h>
#include <MBmaster.h>
#include <string>

using namespace std;
using namespace NCURSES;

const string portFile = "/dev/ttyUSB0";

int main() 
{
   setlocale (LC_ALL, "");

   wstring boudrate[] = {
      L"boudrate", L"9600", L"19200", L"38400", L"57600", L"115200"
   };
   wstring parity[] = {
      L"проверка", L"отсутсвует", L"на чётность", L"на нечётность"
   };
   wstring stopBits[] = {
      L"стоп биты", L"1", L"2"
   };
   wstring open[] = {
      L"открыть", L"закрыть"
   };
   wstring connect[] = {
      L"подключиться", L"отключиться"
   };

   //system("resize -s 0 0");
   system("wmctrl -r :ACTIVE: -b add,fullscreen");
   initscr();      // Переход в curses-режим
   curs_set (0);   // невидимый курсор
   noecho();       // не отображать вводимые символы
   keypad (stdscr, true);
   ColorPairInit();

   auto boudrateMenu = PullDownMenu(boudrate,  0, 0);
   auto parityMenu   = PullDownMenu(parity,    0, boudrateMenu.weight);
   auto stoBitsMenu  = PullDownMenu(stopBits,  0, parityMenu.weight + parityMenu.posX);
   auto addressValue = ChangeValMenu(L"адрес", 0, stoBitsMenu.weight + stoBitsMenu.posX,
                                     1, 1, 255 );
   auto openBut      = Button ( open, 4, 0);
   auto connectBut   = Button ( connect, 4, openBut.weight);

   uint8_t menuQty = 6;

   Iwidget* menu[menuQty] = {
      &boudrateMenu,
      &parityMenu,
      &stoBitsMenu,
      &addressValue,
      &openBut,
      &connectBut
   };

   auto stream = ModbusStreamViever (addressValue.weight + addressValue.posX + 1, 0);
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
         wchar_t key;
         key = getch();
         switch (key) {
            case KEY_UP:
               if ( menu[current]->isEnter() )
                  menu[current]->upHandler();
               else {
                  menu[current]->drawCurrent(color::black);
                  current = 0;
                  menu[current]->drawCurrent(color::green);
               }
               break;
            case KEY_DOWN:
               if ( menu[current]->isEnter() )
                  menu[current]->downHandler();
               else {
                  menu[current]->drawCurrent(color::black);
                  current = 4;
                  menu[current]->drawCurrent(color::green);
               }
               break;
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
            default: menu[current]->enterValHandler(key);  break;

         } // switch (key)

         if ( connectBut.push ) {
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
            connectBut.push = false;
         }
         break;

      case tryConnect:
         int devAdr, regAdr;
         devAdr = addressValue.value;
         regAdr = 0;
         modbus.tx_rx (MBfunc::Read_Registers_03, devAdr, regAdr, 4);
         if ( modbus.state == MBstate::doneNoErr ) {
            state = startMenu;
            connectBut.push = false;
         } else if ( modbus.isError() ) {
            state = startMenu;
            connectBut.push = false;
         }
         break;
      } // switch (state)

   } // while (1)



   // getch();
   endwin();       // Выход из curses-режима. Обязательная команда.

   return 0; 
}