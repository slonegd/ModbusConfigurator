#pragma once

#include <stdint.h>
#include <cstring>
//#include <ncurses.h>
#include "ncursesMenu.h"
#include <tuple>
#include <crc.h>

class MBmaster
{
public:
   uint8_t buf[255];
   uint8_t byteQty;
   ModbusStreamViever& stream;

   MBmaster (ModbusStreamViever& stream)
      : stream(stream), state(pack)
   {   }

	enum State {            // состояние класса
      funcErr 	     = 1,   // из спецификации на модбас
		regErr		  = 2,   // из спецификации на модбас
		valueErr      = 3,   // из спецификации на модбас
		CRCerr,              // битый ответ
		timeoutErr,	
		doneNoErr,           // всё норм, можно работать дальше
      pack,                // упаковка пакета
      transmit,
	} state;
   enum MBfunc {
      Read_Registers_03    = 3,
      Force_Single_Coil_05 = 5,
      Write_Registers_16   = 16
   };


   template<typename ... Types>
   void tx_rx (
      MBfunc f,
      int devAdr,
      int regAdr,
      // перечисление значений регистров для записи, начиная с regAdr
      // или количество регистров для чтения
      Types ... args )
   {
      auto arg0 = get<0>(make_tuple(args...));
      uint16_t crc;

      while (1) {
         switch (state) {

         case doneNoErr:
         case timeoutErr:
         case CRCerr:
         case funcErr:
         case regErr:
         case valueErr:
            state = pack;
            break;


         case pack:
            buf[0] = devAdr;
            buf[1] = f;
            buf[2] = regAdr >> 8;
            buf[3] = regAdr;
            byteQty = 4;
            if (f == Force_Single_Coil_05) {
               buf[4] = arg0 ? 0xFF : 0x00;
               buf[5] = 0x00;
               byteQty = 6;
            } else if (f == Read_Registers_03) {
               buf[4] = 0;
               buf[5] = arg0;
               byteQty = 6;
            } else if (f == Write_Registers_16) {
               byteQty = 7;
               for (auto&& arg : initializer_list<uint16_t>{(uint16_t)args...} ) {
                  buf[byteQty++] = arg >> 8;
                  buf[byteQty++] = arg;
               }
               buf[4] = 0;
               buf[5] = (byteQty - 7) / 2;
               buf[6] = byteQty - 7;
            }
            crc = crc16 (buf, byteQty);
            buf[byteQty++] = crc;
            buf[byteQty++] = crc >> 8;
            stream.addData (buf, byteQty, NCURSES::color::tBlue);
            stream.endLine();
            state = transmit;
            break;
            

         case transmit:
            return;

            break;
         } // switch (state)
      } // while (1)

   };

   enum Boudrate {
      br9600   = 9600,
      br14400  = 14400,
      br19200  = 19200,
      br28800  = 28800,
      br38400  = 38400,
      br57600  = 57600,
      br76800  = 76800,
      br115200 = 115200
   };
   enum Parity {
      none,
      even,
      odd
   };
   bool openPort (
      Boudrate boudrate,
      Parity parity,
      int stopBits,
      int msTimeout
   ) {}
/*
private:
   template <class T, typename ... Types>
   initializer_list<uint16_t> getArgsWithoutFirst (T arg0, Types ...args)
   {
      return initializer_list<uint16_t>{(uint16_t)args...};
   }
*/
};