#include <init.h>

using namespace std;
using namespace NCURSES;

uint16_t device {0};

bool done {false};

int main() 
{
   setlocale (LC_ALL, "");
   
   system("resize -s 24 180");
   //system("wmctrl -r :ACTIVE: -b add,fullscreen");
   initscr();      // Переход в curses-режим
   curs_set (0);   // невидимый курсор
   noecho();       // не отображать вводимые символы
   keypad (stdscr, true);
   halfdelay(1);
   ColorPairInit();

   auto draw = [] (auto menu, int begin, int qty) {
      for (int i = begin; i < qty + begin; i++)
         menu[i]->draw();
   };
   stream.draw();

   // указывает на текущее выбранное меню
   int current = 0;
   
   enum StateWork {
      startMenu,
      openPort,
      tryConnect,
      MBSet,
      dimmer,
      dynamic,
      measure,
      pro1,
      si
   } state = startMenu;
   // для определения смены состояний LT - LastTime
   StateWork stateLT = state;
   // true при смене состояния, для реализации входных действий
   // вроде прорисовки
   volatile bool enterState = true;

   bool work = true;
   while (work) {
      uint8_t buf[255];

      wchar_t key;
      key = getch();

      switch (state) {
///////////////////////////////////////////////////////////////////////////////
//
//    ГЛАВНОЕ МЕНЮ
//
///////////////////////////////////////////////////////////////////////////////
      case startMenu:

         if (enterState) {
            draw(menu, 0, menuQty);
            menu[current]->drawCurrent(color::green);
         }

         if (modbus.port.open__)
            if ( modbus.port.read_ (buf) && !modbus.port.isTimeout() )
               stream.addData (buf, modbus.port.getReadQty(), NCURSES::color::tGreen);

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
///////////////////////////////////////////////////////////////////////////////
//
//    МЕНЮ НАСТРОЕК СЕТИ
//
///////////////////////////////////////////////////////////////////////////////
      case tryConnect:
         int regAdr;
         regAdr = 0;
         modbus.tx_rx (MBfunc::Read_Registers_03, addressValue.value, regAdr, 4);
         if ( modbus.state == MBstate::doneNoErr or debug ) {
               state = MBSet;
               set.val = modbus.readBuf[2];
               boudrateSet.curChoice = set.boud == br9600  ? 1 : 
                                       set.boud == br19200 ? 2 :
                                       set.boud == br38400 ? 3 :
                                       set.boud == br57600 ? 4 : 5;
               paritySet.curChoice = set.parity == none ? 1 :
                                     set.parity == even ? 2 : 3;
               stopBitsSet.curChoice = set.bits == _1 ? 1: 2;
               addressSet.value = modbus.readBuf[3];
               scr_dump("./screens/main");
               table.draw();
               draw (menuSet, 2, arrSize(menuSet) - 2);
               current = 1;
               device = debug ? 6 : modbus.readBuf[0];

            if (device == 4) {
               label.setLabel1 (L"Подключено к ЭО-81 Контроллер УОВ");
               label.setLabel2 (L"прошивка v1.00");
            } else if (device == 2) {
               label.setLabel1 (L"Подключено к ЭО-67 Диммер v4");
               label.setLabel2 (L"прошивка v1.00");
            } else if (device == 1) {
               label.setLabel1 (L"Подключено к ЭО-74 ДУГ");
               label.setLabel2 (L"прошивка v1.00");     
            } else if (device == 5) {
               label.setLabel1 (L"Подключено к ЭО-84 ген с измерениями");
               label.setLabel2 (L"прошивка v1.00");
            } else if (device == 6) {
               label.setLabel1 (L"Подключено к ЭО-72 цифровой прототип 1");
               label.setLabel2 (L"прошивка v1.00");
            } else if (device == 7) {
               label.setLabel1 (L"Подключено к ЭО-94 счётчик импульсов");
               label.setLabel2 (L"");
            } else {
               state = startMenu;
               connectBut.unPush();
            }
         } else if ( modbus.isError() ) {
            state = startMenu;
            connectBut.unPush();
         }
         break;

      case MBSet:
         if (enterState) {
            label.draw();
         }
         switch (key) {
            case KEY_UP:
               if (menuSet[current]->isEnter() )
                 menuSet[current]->upHandler();
               else if (current > 1) {
                 menuSet[current]->drawCurrent(color::black);
                  current--;
                 menuSet[current]->drawCurrent(color::green);
               }
               break;
            case KEY_DOWN:
               if (menuSet[current]->isEnter() )
                 menuSet[current]->downHandler();
               else if (current < arrSize(menuSet) - 1) {
                 menuSet[current]->drawCurrent(color::black);
                  current++;
                 menuSet[current]->drawCurrent(color::green);
               }
               break;
            case '\n':    menuSet[current]->enterHandler(); break;
            case KEY_END: work = false; break; 
            default:menuSet[current]->enterValHandler(key);  break;
         }
         if ( !connectBut.isPush() ) {
            scr_restore("./screens/main");
            stream.clean();
            current = 5;
            state = startMenu;
         }
         if (saveBut.isPush()) {
            int regAdr;
            regAdr = 0;
            set.boud = boudrateSet.curChoice == 1 ? br9600  :
                       boudrateSet.curChoice == 2 ? br19200 :
                       boudrateSet.curChoice == 3 ? br38400 :
                       boudrateSet.curChoice == 4 ? br57600 : br115200;
            set.parity = paritySet.curChoice == 1 ? none :
                         paritySet.curChoice == 2 ? even : odd;
            set.bits = stopBitsSet.curChoice == 1 ? _1 : _2;
            modbus.tx_rx (MBfunc::Write_Registers_16, addressValue.value, regAdr,
                          set.val, addressSet.value);
            if ( modbus.state == MBstate::doneNoErr || modbus.isError() )
               saveBut.unPush();
         }
         if (addBut.isPush()) {
            state = device == 1 ? dynamic :
                    device == 2 ? dimmer  :
                    device == 4 ? MBSet   : // placeholder
                    device == 5 ? measure :
                    device == 6 ? pro1    : MBSet;
            addBut.unPush();
         }
      break;
///////////////////////////////////////////////////////////////////////////////
//
//    МЕНЮ ДИММЕРА
//
///////////////////////////////////////////////////////////////////////////////
      case dimmer:
         if (enterState) {
            scr_dump("./screens/mbSet");
            dimTable.draw();
            draw (dimSet, 0, arrSize(dimSet) - 1);
            current = 1;
            label.draw();
         }
         modbus.tx_rx (MBfunc::Write_Registers_16, addressValue.value, 4, dSet.value);
         switch (key) {
            case KEY_UP:
               if (dimSet[current]->isEnter() )
                 dimSet[current]->upHandler();
               else if (current > 1) {
                 dimSet[current]->drawCurrent(color::black);
                  current--;
                 dimSet[current]->drawCurrent(color::green);
               }
               break;
            case KEY_DOWN:
               if (dimSet[current]->isEnter() )
                 dimSet[current]->downHandler();
               else if (current < arrSize(dimSet) - 1) {
                 dimSet[current]->drawCurrent(color::black);
                  current++;
                 dimSet[current]->drawCurrent(color::green);
               }
               break;
            case '\n':    dimSet[current]->enterHandler(); break;
            case KEY_END: work = false; break; 
            default:dimSet[current]->enterValHandler(key);  break;
         }
         if (outBut.isPush()) {
            scr_restore("./screens/mbSet");
            stream.clean();
            current = 1;
            state = startMenu;
         }
      break;
///////////////////////////////////////////////////////////////////////////////
//
//    МЕНЮ ДУГ
//
///////////////////////////////////////////////////////////////////////////////
      case dynamic:
         if (enterState) {
            scr_dump("./screens/mbSet");
            clear();
            // dynTable.draw();
            draw (dymSet, 0, arrSize(dymSet) - 1);
            transmitBut.draw();
            current = 1;
            label.draw();
         }
         switch (key) {
            case KEY_UP:
               if (dymSet[current]->isEnter() )
                 dymSet[current]->upHandler();
               else if (current > 1) {
                 dymSet[current]->drawCurrent(color::black);
                  current--;
                 dymSet[current]->drawCurrent(color::green);
               }
               break;
            case KEY_DOWN:
               if (dymSet[current]->isEnter() )
                 dymSet[current]->downHandler();
               else if (current < arrSize(dymSet) - 1) {
                 dymSet[current]->drawCurrent(color::black);
                  current++;
                 dymSet[current]->drawCurrent(color::green);
               }
               break;
            case '\n':    dymSet[current]->enterHandler(); break;
            case KEY_END: work = false; break; 
            default:dymSet[current]->enterValHandler(key);  break;
         }
         if (transmitBut.isPush()) {
            int regAdr;
            regAdr = 4;
            modbus.tx_rx (MBfunc::Write_Registers_16, addressValue.value, regAdr,
                          dynFreq.value);
            if ( modbus.state == MBstate::doneNoErr || modbus.isError() )
               transmitBut.unPush();
         }
         if ( !connectBut.isPush() ) {
            scr_restore("./screens/main");
            stream.clean();
            current = 5;
            state = startMenu;
         }
      break;
///////////////////////////////////////////////////////////////////////////////
//
//    МЕНЮ ГЕНА С ИЗМЕРЕНИЯМИ
//
///////////////////////////////////////////////////////////////////////////////
      case measure:
         if (enterState) {
            scr_dump("./screens/mbSet");
            clear();
            frequencyLabel.draw();
            currentLabel.draw();
            draw (measureAdd, 0, arrSize(measureAdd));
            current = 1;
            measureAdd[current]->drawCurrent(color::green);
            label.draw();
         }
         modbus.tx_rx (MBfunc::Read_Registers_03, addressValue.value, 4, 2);
            if ( modbus.state == MBstate::doneNoErr ) {
               frequencyLabel.setLabel1 (L"Частота " + to_wstring(modbus.readBuf[0]));
               currentLabel.setLabel1   (L"Ток "     + to_wstring(modbus.readBuf[1]));
               frequencyLabel.draw();
               currentLabel.draw();
            }

         switch (key) {
            case KEY_UP:
               if (measureAdd[current]->isEnter() )
                 measureAdd[current]->upHandler();
               else if (current > 1) {
                 measureAdd[current]->drawCurrent(color::black);
                 current--;
                 measureAdd[current]->drawCurrent(color::green);
               }
               break;
            case KEY_DOWN:
               if (measureAdd[current]->isEnter() )
                 measureAdd[current]->downHandler();
               else if (current < arrSize(measureAdd) - 1) {
                 measureAdd[current]->drawCurrent(color::black);
                 current++;
                 measureAdd[current]->drawCurrent(color::green);
               }
               break;
            case '\n':    measureAdd[current]->enterHandler(); break;
            case KEY_END: work = false; break; 
            default:measureAdd[current]->enterValHandler(key);  break;
         }

         if ( !connectBut.isPush() ) {
            scr_restore("./screens/main");
            stream.clean();
            current = 5;
            state = startMenu;
         }
      break;
///////////////////////////////////////////////////////////////////////////////
//
//    МЕНЮ ЦИФРОВОГО ПРОТОТИП 1
//
///////////////////////////////////////////////////////////////////////////////
      case pro1:
         if (enterState) {
            scr_dump("./screens/mbSet");
            clear();
            PRO1::frequencyLabel.draw();
            PRO1::ratioLabel    .draw();
            PRO1::powerLabel    .draw();
            PRO1::setLabel      .draw();
            draw (measureAdd, 0, arrSize(measureAdd));
            current = 1;
            measureAdd[current]->drawCurrent(color::green);
            label.draw();
         }
         
         modbus.tx_rx (MBfunc::Read_Registers_03, addressValue.value, PRO1::adr, PRO1::cnt);
         if ( modbus.state == MBstate::doneNoErr ) {
            PRO1::frequencyLabel.setLabel1 (
                L"Частота min: " + to_wstring(modbus.readBuf[9] - modbus.readBuf[8])
              + L" текущая: "    + to_wstring(modbus.readBuf[0])
              + L" max: "        + to_wstring(modbus.readBuf[9] + modbus.readBuf[7])
            );
            PRO1::ratioLabel    .setLabel1 (L"Коэффициент "  + to_wstring(modbus.readBuf[1]) + L"/1000");
            PRO1::powerLabel    .setLabel1 (L"Мощность "     + to_wstring(modbus.readBuf[2]));
            PRO1::setLabel      .setLabel1 (
               modbus.readBuf[6] == 0 ? L"Включить/выключить " :
               modbus.readBuf[6] == 1 ? L"Задание частоты "      + to_wstring(modbus.readBuf[0]) : 
               modbus.readBuf[6] == 2 ? L"Задание частоты х10 "  + to_wstring(modbus.readBuf[0]) : 
               modbus.readBuf[6] == 3 ? L"Задание частоты х100 " + to_wstring(modbus.readBuf[0]) : 
               modbus.readBuf[6] == 4 ? L"Задание мощности "     + to_wstring(modbus.readBuf[5]) : 
               modbus.readBuf[6] == 5 ? L"Задание мощности х10 " + to_wstring(modbus.readBuf[5]) : 
               modbus.readBuf[6] == 6 ? L"Задание +перестройки " + to_wstring(modbus.readBuf[7]) : 
               modbus.readBuf[6] == 7 ? L"Задание -перестройки " + to_wstring(modbus.readBuf[8]) : 
            L"");
            PRO1::frequencyLabel.draw();
            PRO1::ratioLabel    .draw();
            PRO1::powerLabel    .draw();
            PRO1::setLabel      .draw();

         }
       
         switch (key) {
            case KEY_UP:
               if (measureAdd[current]->isEnter() )
                 measureAdd[current]->upHandler();
               else if (current > 1) {
                 measureAdd[current]->drawCurrent(color::black);
                 current--;
                 measureAdd[current]->drawCurrent(color::green);
               }
               break;
            case KEY_DOWN:
               if (measureAdd[current]->isEnter() )
                 measureAdd[current]->downHandler();
               else if (current < arrSize(measureAdd) - 1) {
                 measureAdd[current]->drawCurrent(color::black);
                 current++;
                 measureAdd[current]->drawCurrent(color::green);
               }
               break;
            case '\n':    measureAdd[current]->enterHandler(); break;
            case KEY_END: work = false; break; 
            default: measureAdd[current]->enterValHandler(key);  break;
         }

         if ( !connectBut.isPush() ) {
            scr_restore("./screens/main");
            stream.clean();
            current = 5;
            state = startMenu;
         }
         break;
///////////////////////////////////////////////////////////////////////////////
//
//    СЧЁТЧИК ИМПУЛЬСОВ
//
///////////////////////////////////////////////////////////////////////////////
      case si:
         if (enterState) {
            scr_dump("./screens/mbSet");
            clear();
            SI::coordinate.draw();
            SI::sensor    .draw();
            SI::state     .draw();
            draw (measureAdd, 0, arrSize(measureAdd));
            current = 1;
            measureAdd[current]->drawCurrent(color::green);
            label.draw();
         }
         modbus.tx_rx (MBfunc::Read_Registers_03, addressValue.value, SI::adr, SI::cnt);
         if ( modbus.state == MBstate::doneNoErr ) {
            SI::coordinate.setLabel1 (L"Координата: " + to_wstring(modbus.readBuf[0]));
            std::wstring tmp{};
            if (bool(modbus.readBuf[1] & 0b000001)) tmp += L"right ";
            if (bool(modbus.readBuf[1] & 0b000010)) tmp += L"left ";
            if (bool(modbus.readBuf[1] & 0b000100)) tmp += L"till ";
            if (bool(modbus.readBuf[1] & 0b001000)) tmp += L"begin ";
            if (bool(modbus.readBuf[1] & 0b010000)) tmp += L"hi ";
            if (bool(modbus.readBuf[1] & 0b100000)) tmp += L"lo ";
            SI::sensor.setLabel1 (tmp);
            tmp = (modbus.readBuf[2] & 0b111) == 0b000 ? L"wait " :
                  (modbus.readBuf[2] & 0b111) == 0b001 ? L"auto " :
                  (modbus.readBuf[2] & 0b111) == 0b010 ? L"manual " :
                  (modbus.readBuf[2] & 0b111) == 0b011 ? L"search " :
                  (modbus.readBuf[2] & 0b111) == 0b100 ? L"calib " :
                  (modbus.readBuf[2] & 0b111) == 0b101 ? L"avaria " : L"";
            if (bool(modbus.readBuf[2] & (1 <<  3))) tmp += L"up ";
            if (bool(modbus.readBuf[2] & (1 <<  4))) tmp += L"down ";
            if (bool(modbus.readBuf[2] & (1 <<  5))) tmp += L"right ";
            if (bool(modbus.readBuf[2] & (1 <<  6))) tmp += L"left ";
            if (bool(modbus.readBuf[2] & (1 <<  7))) tmp += L"fast_stop ";
            if (bool(modbus.readBuf[2] & (1 <<  8))) tmp += L"slow_stop ";
            if (bool(modbus.readBuf[2] & (1 <<  9))) tmp += L"hi_speed ";
            if (bool(modbus.readBuf[2] & (1 << 10))) tmp += L"lo_speed ";
            if (bool(modbus.readBuf[2] & (1 << 11))) tmp += L"h_stop ";
            if (bool(modbus.readBuf[2] & (1 << 12))) tmp += L"v_stop ";
            if (bool(modbus.readBuf[2] & (1 << 13))) tmp += L"enable ";
            if (bool(modbus.readBuf[2] & (1 << 13))) tmp += L"lost ";
            SI::state.setLabel1 (tmp);
            SI::coordinate.draw();
            SI::sensor    .draw();
            SI::state     .draw();
         }
       
         switch (key) {
            case KEY_UP:
               if (measureAdd[current]->isEnter() )
                 measureAdd[current]->upHandler();
               else if (current > 1) {
                 measureAdd[current]->drawCurrent(color::black);
                 current--;
                 measureAdd[current]->drawCurrent(color::green);
               }
               break;
            case KEY_DOWN:
               if (measureAdd[current]->isEnter() )
                 measureAdd[current]->downHandler();
               else if (current < arrSize(measureAdd) - 1) {
                 measureAdd[current]->drawCurrent(color::black);
                 current++;
                 measureAdd[current]->drawCurrent(color::green);
               }
               break;
            case '\n':    measureAdd[current]->enterHandler(); break;
            case KEY_END: work = false; break; 
            default: measureAdd[current]->enterValHandler(key);  break;
         }

         if ( !connectBut.isPush() ) {
            scr_restore("./screens/main");
            stream.clean();
            current = 5;
            state = startMenu;
         }
         break;
///////////////////////////////////////////////////////////////////////////////
//
//    КОНЕЦ
//
///////////////////////////////////////////////////////////////////////////////
      // default: ;
      } // switch (state)

      enterState = state != stateLT;
      stateLT = state;
   } // while (1)






   // getch();
   endwin();       // Выход из curses-режима. Обязательная команда.

   return 0; 
}
