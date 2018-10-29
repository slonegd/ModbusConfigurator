#pragma once

#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include "ncursesWidgets.h"
#include "MBmaster.h"
#include <string>

template<class T, int n> int arrSize (T (&) [n]) { return n; }

using namespace std;
using namespace NCURSES;

const string portFile = "/dev/ttyUSB0";

const bool debug =
#if defined DEBUG
   true;
#else
   false;
#endif


// основное меню
wstring boudrate[] = {
   L"boudrate", L"9600", L"19200", L"38400", L"57600", L"115200"
};
wstring parity[] = {
   L"проверка", L"отсутсвует", L"на чётность", L"на нечётность"
};
wstring stopBits[] = {
   L"стоп биты", L"1", L"2"
};
wstring open_[] = {
   L"открыть", L"закрыть"
};
wstring connect[] = {
   L"подключиться", L"отключиться"
};
auto boudrateMenu = PullDownMenu(boudrate,  0, 0);
auto parityMenu   = PullDownMenu(parity,    0, boudrateMenu.weight);
auto stoBitsMenu  = PullDownMenu(stopBits,  0, parityMenu.weight + parityMenu.posX);
auto addressValue = ChangeValMenu(L"адрес", 0, stoBitsMenu.weight + stoBitsMenu.posX,
                                    1, 1, 255 );
auto openBut      = Button ( open_, 4, 0);
auto connectBut   = Button ( connect, 4, openBut.weight);
//uint8_t const menuQty = 6;
Iwidget* menu[] = {
   &boudrateMenu,
   &parityMenu,
   &stoBitsMenu,
   &addressValue,
   &openBut,
   &connectBut
};
uint8_t menuQty = arrSize (menu);

// инфопоток модбаса и сам модбас
auto stream = ModbusStreamViever (addressValue.weight + addressValue.posX);
auto modbus = MBmaster(stream, portFile);
using MBstate = MBmaster::State;
using MBfunc = MBmaster::MBfunc;
enum BoudrateSet {
   br9600   = 0b000,
   br14400  = 0b001,
   br19200  = 0b010,
   br28800  = 0b011,
   br38400  = 0b100,
   br57600  = 0b101,
   br76800  = 0b110,
   br115200 = 0b111
};
enum ParitySet {
   none = 0b00,
   even = 0b11,
   odd  = 0b01,
};
enum BitsSet {
   _1 = 0b0,
   _2 = 0b1
};
struct MBsettings {
   union {
      struct {
         ParitySet   parity   :2;
         BitsSet     bits     :1;
         BoudrateSet boud     :3;
      };
      uint16_t val;
   };
   MBsettings() : val(0) { }
};
auto set = MBsettings();


// таблица параметров
const int nameWeight = 35;
const int valWeight = 15;
auto table = Table (7, 0, nameWeight, valWeight, 4);
auto boudrateSet = PullDownMenu(boudrate, table, 0);
auto paritySet   = PullDownMenu(parity,   table, 1);
auto stopBitsSet = PullDownMenu(stopBits, table, 2);
auto addressSet  = ChangeValMenu(L"адрес", table, 3, 1, 1, 255);
auto saveBut     = Button(L"сохранить", table.y + table.height, nameWeight + 5);
auto addBut      = Button(L"дополнительно", saveBut.posY + 3, nameWeight + 1);
Iwidget* menuSet[] = {
   &openBut,
   &connectBut,
   &boudrateSet,
   &paritySet,
   &stopBitsSet,
   &addressSet,
   &saveBut,
   &addBut
};


// название
auto label = Label (0, 0, stream.posX);


// диммер
auto dimTable = Table (7, 0, nameWeight, valWeight, 1);
auto dSet   = ChangeValMenu(L"Коэффициент заполнения (0-255)", table, 0, 0, 0, 255);
auto outBut = Button(L"выход", dimTable.y + dimTable.height, nameWeight + 9);
Iwidget* dimSet[] = {
   &openBut,
   &connectBut,
   &dSet,
   &outBut
};


// ДУГ
auto dynTable = Table (7, 0, nameWeight, valWeight, 1);
auto dynFreq  = ChangeValMenu(L"Частота в дГц (0-5000)", table, 0, 0, 0, 50000);
auto transmitBut = Button(L"передать", dynTable.y + dynTable.height, nameWeight + 6);
Iwidget* dymSet[] = {
   &openBut,
   &connectBut,
   &dynFreq,
   &transmitBut
};

// Ген с измерениями
auto frequencyLabel = Label (7  , 0, stream.posX);
auto currentLabel   = Label (7+4, 0, stream.posX);
Iwidget* measureAdd[] = {
   &openBut,
   &connectBut
};

// Ген прототип
namespace PRO1 {
auto frequencyLabel = Label (7 + 4*0, 0, stream.posX);
auto ratioLabel     = Label (7 + 4*1, 0, stream.posX);
auto powerLabel     = Label (7 + 4*2, 0, stream.posX);
auto setLabel       = Label (7 + 4*3, 0, stream.posX);
auto constexpr adr {4};
auto constexpr cnt {10};
}


namespace SI {
auto coordinate = Label (7 + 4*0, 0, stream.posX);
auto sensor     = Label (7 + 4*1, 0, stream.posX);
auto state      = Label (7 + 4*2, 0, stream.posX);
auto constexpr adr {6};
auto constexpr cnt {3};
}
