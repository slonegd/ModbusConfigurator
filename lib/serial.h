#pragma once

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <stdint.h>
#include <sys/select.h>

class SerialPort
{
public:
   bool open__;

   SerialPort (std::string portFile) 
      : open__(false), portFile(portFile), descriptor(-1), work(false)
   {  }

   const std::string portFile;

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

   bool open_ (
      Boudrate boudrate,
      Parity parity,
      int stopBits,
      int msTimeout )
   {
      close_();

      descriptor = open (portFile.c_str(), O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
      if (descriptor == -1)
         return false;
     else
         fcntl(descriptor, F_SETFL, FNDELAY);

      termios attr;
      tcgetattr (descriptor, &attr);
      speed_t boud = boudrate == br9600  ? B9600  :
                  // boudrate == br14400 ? B14400 :
                     boudrate == br19200 ? B19200 :
                  // boudrate == br28800 ? B28800 :
                     boudrate == br38400 ? B38400 :
                     boudrate == br57600 ? B57600 :
                  // boudrate == br76800 ? B76800 :
                                           B115200;
      cfsetispeed (&attr, boud);
      cfsetospeed (&attr, boud);
      if (stopBits == 2)
         attr.c_cflag |= CSTOPB;
      else
         attr.c_cflag &= ~CSTOPB;
      if (parity == none)
         attr.c_cflag &= ~PARENB;
      else
         attr.c_cflag |= PARENB;
      if (parity == even)
         attr.c_cflag &= ~PARODD;
      else
         attr.c_cflag |= PARODD;
      attr.c_cflag &= ~CSIZE;
      attr.c_cflag |= CLOCAL | CREAD | CS8;
      // raw mode
      attr.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG );
      attr.c_oflag &= ~OPOST;

      attr.c_iflag &= ~( IXON | IXOFF | IXANY | INLCR | IGNCR | ICRNL );

      tcsetattr (descriptor, TCSANOW, &attr);

      open__ = true;
      usEndMes = 35 * 1000000 / (int)boudrate;
      
      return true;
   }

   void close_()
   {
      if (open__) {
         while (close (descriptor) != 0) { }
      }
      open__ = false;
   }


   bool write_ (uint8_t* buf, int qty)
   {
      uint8_t buf2[255];
      while ( read (descriptor, buf2, 255) > 0) {}
      return qty == write (descriptor, buf, qty);
   }


   bool read_ (uint8_t* buf)
   {
      fd_set fds;
      timeval timeoutVal = { 0 };

      while (1) {
         if ( !work ) {
            byteQty = 0;
            timeout = false;
            work = true;      
         } else {
            FD_ZERO (&fds);
            FD_SET (descriptor, &fds);
            timeoutVal.tv_usec = byteQty == 0 ? 50000 : usEndMes;
            if ( select (descriptor + 1, &fds, NULL, NULL, &timeoutVal) ) {
               byteQty += read (descriptor, &buf[byteQty], 255);
            } else {
                if ( byteQty == 0)
                  timeout = true;
               work = false;
               return true; 
            }
         } // work / !work
      } // while (1)
   }


   int getReadQty() { return byteQty; }

   bool isTimeout() { return timeout; }


private:
   int descriptor;
   int byteQty;
   bool timeout;
   bool work;
   int usEndMes;
};