#include <init.h>

using namespace std;
using namespace NCURSES;

int main() 
{
   setlocale (LC_ALL, "");
   
   //system("resize -s 0 0");
   system("wmctrl -r :ACTIVE: -b add,fullscreen");
   initscr();      // Переход в curses-режим
   curs_set (0);   // невидимый курсор
   noecho();       // не отображать вводимые символы
   keypad (stdscr, true);
   halfdelay(1);
   ColorPairInit();

   uint8_t menuQty = 6;
   Iwidget* menu[menuQty] = {
      &boudrateMenu,
      &parityMenu,
      &stoBitsMenu,
      &addressValue,
      &openBut,
      &connectBut
   };
   for (int i = 0; i < menuQty; i++)
      menu[i]->draw();
   stream.draw();

   int current = 0;
   menu[current]->drawCurrent(color::green);

   bool work = true;

   enum StateWork {
      startMenu,
      openPort,
      tryConnect
   } state = startMenu;

   while (work) {
      uint8_t buf[255];

      if (modbus.port.open__ && (state == startMenu) )
         if ( modbus.port.read_ (buf) && !modbus.port.isTimeout() )
            stream.addData (buf, modbus.port.getReadQty(), NCURSES::color::tGreen);


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

         if ( openBut.isPush() && !modbus.port.open__ ) {
            state = openPort;
         } else if (!openBut.isPush() && modbus.port.open__) {
            modbus.port.close_();
            stream.addString ("Close " + portFile, NCURSES::color::tGreen);
         } else if ( connectBut.isPush() ) {
            if (modbus.port.open__) {
               state = tryConnect;
            } else {
               stream.addString (portFile + " is closed", color::tRed);
               connectBut.unPush();
            }
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
            stream.addString ("Open " + portFile, NCURSES::color::tGreen);
            modbus.state = MBmaster::State::pack;
         } else {
            stream.addString ("Can`t open " + portFile, NCURSES::color::tRed);
            openBut.unPush();
         }
         state = startMenu;
         break;

      case tryConnect:
         int devAdr, regAdr;
         devAdr = addressValue.value;
         regAdr = 0;
         modbus.tx_rx (MBfunc::Read_Registers_03, devAdr, regAdr, 4);
         if ( modbus.state == MBstate::doneNoErr ) {
            state = startMenu;
            connectBut.unPush();
         } else if ( modbus.isError() ) {
            state = startMenu;
            connectBut.unPush();
         }
         break;
      } // switch (state)

   } // while (1)



   // getch();
   endwin();       // Выход из curses-режима. Обязательная команда.

   return 0; 
}