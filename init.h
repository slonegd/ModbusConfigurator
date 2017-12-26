#pragma once

#include <stdint.h>
#include <locale.h>
#include <wchar.h>
#include <ncursesWidgets.h>
#include <MBmaster.h>
#include <string>

using namespace std;
using namespace NCURSES;

const string portFile = "/dev/ttyUSB0";


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


// инфопоток модбаса и сам модбас
auto stream = ModbusStreamViever (addressValue.weight + addressValue.posX + 1);
auto modbus = MBmaster(stream, portFile);
using MBstate = MBmaster::State;
using MBfunc = MBmaster::MBfunc;


// меню задания параметров модбаса