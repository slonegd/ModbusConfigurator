#pragma once

#include <cstring>
#include <ncursesWidgets.h>
#include <serial.h>
#include <crc.h>

class MBmaster
{
public:
   uint16_t readBuf[255];
   ModbusStreamViever& stream;
   SerialPort port;

   MBmaster (ModbusStreamViever& stream, std::string portFile)
      : stream(stream), port(SerialPort(portFile)), state(pack)//, descriptor(3)
   {   }

	enum State {            // состояние класса
      funcErr       = 1,   // из спецификации на модбас
      regErr        = 2,   // из спецификации на модбас
      valueErr      = 3,   // из спецификации на модбас
      CRCerr,              // битый ответ
      answerErr,           // другие и неизвестные ошибки ответа
      notFullAnswerErr,    // размер ответа меньше ожидаемого
      unknowErr,
      timeoutErr,
      doneNoErr,           // всё норм, можно работать дальше
      pack,                // упаковка пакета
      transmit,
      receive,
      handle03,
      handle05,
      handle16
	} state;
   enum MBfunc {
      Read_Registers_03    = 3,
      Force_Single_Coil_05 = 5,
      Write_Registers_16   = 16
   };

   using Boudrate = SerialPort::Boudrate;
   using Parity = SerialPort::Parity;

   template<typename ... Types>
   void tx_rx (
      MBfunc f,
      int devAdr,
      int regAdr,
      // перечисление значений регистров для записи, начиная с regAdr
      // или количество регистров для чтения
      Types ... args )
   {
      uint16_t crc;

      while (1) {
         switch (state) {
            case doneNoErr:
            case timeoutErr:
            case CRCerr:
            case funcErr:
            case regErr:
            case valueErr:
            case notFullAnswerErr:
            case unknowErr:
               state = pack;
               break;

            case pack:
               buf[0] = devAdr;
               buf[1] = f;
               buf[2] = regAdr >> 8;
               buf[3] = regAdr;
               byteQty = 4;
               if (f == Force_Single_Coil_05) {
                  buf[4] = getArg1(args...) ? 0xFF : 0x00;
                  buf[5] = 0x00;
                  byteQty = 6;
               } else if (f == Read_Registers_03) {
                  buf[4] = 0;
                  buf[5] = getArg1(args...);
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
               state = transmit;
               break;

            case transmit:
               if ( port.write_ (buf, byteQty) ) {
                  byteQty = 0;
                  state = receive;              
               }
               return;
               break;

            case receive:
               if ( port.read_ (buf) ) {
                  if ( port.isTimeout() ) {
                     stream.addString ("Timeout", color::tRed);
                     state = timeoutErr;
                  } else {
                     byteQty = port.getReadQty();
                     stream.addData (buf, byteQty, NCURSES::color::tGreen);
                     state = f == Read_Registers_03    ? handle03 :
                             f == Force_Single_Coil_05 ? handle05 :
                             f == Write_Registers_16   ? handle16 :
                                                         handle05;
                  }
               }
               return;
               break;

            case handle03:
               if ( byteQty != (2*getArg1(args...) + 5) ) {
                  stream.addString ("Not full answer error", color::tRed);
                  state = notFullAnswerErr;
                  return;
               } else  if ( otherErrors (devAdr) )
                  return;
               for (uint8_t i = 0; i < getArg1(args...); ++i)
                  readBuf[i] = ((uint16_t)(buf[3+2*i]) << 8) | buf[3+2*i+1];
               state = doneNoErr;
               return;
               break;

            case handle05:
            case handle16:
               if ( byteQty != 8 ) {
                  stream.addString ("Not full answer error", color::tRed);
                  state = notFullAnswerErr;
                  return;
               } else  if ( otherErrors (devAdr) )
                  return;
               state = doneNoErr;
               return;
               break;

            default: 
               state = answerErr;
               break;
         } // switch (state)
      } // while (1)
   }


   bool isError()
   {
      return (state >= funcErr) && (state <= timeoutErr);
   }


private:
   uint8_t buf[255];
   volatile uint8_t byteQty;


   template <class T, typename ... Types>
   T getArg1 (T arg1, Types ... otherArgs)
   {
      return arg1;
   }


   bool otherErrors (int& devAdr)
   {
      uint16_t crc = crc16 (buf, byteQty - 2);
      if ( buf[byteQty-1] != (crc >> 8) || buf[byteQty-2] != (crc & 0xFF) ) {
         stream.addString ("CRC error", color::tRed);
         state = CRCerr;
         return true;
      } else if ( buf[0] != devAdr ) {
         stream.addString ("Wrong answer error", color::tRed);
         state = answerErr;
         return true;
      } else if ( (buf[1] & 0b1000'0000) != 0 ) {
         state = (State)buf[2];
         state == funcErr  ? stream.addString ("Wrong function error", color::tRed) :
         state == regErr   ? stream.addString ("Registr address error", color::tRed) :
         state == valueErr ? stream.addString ("Wrong value error", color::tRed) :
                             stream.addString ("Unknow error", color::tRed);
         return true;
      }
      return false;
   }

};