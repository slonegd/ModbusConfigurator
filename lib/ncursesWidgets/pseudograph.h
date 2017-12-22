#pragma once

#include <string>
#include <wchar.h>

namespace PSEUDO {
   wchar_t horisontal() { return L'\x2501'; }
   wchar_t vertical()   { return L'\x2503'; }
   wchar_t upLeft()     { return L'\x250F'; }
   wchar_t upRight()    { return L'\x2513'; }
   wchar_t downLeft()   { return L'\x2517'; }
   wchar_t downRight()  { return L'\x251B'; }

   std::wstring upBorder (int length)
   {
      std::wstring res = L"";
      res += upLeft();
      for (int i = 1; i < length - 1; ++i)
         res += horisontal();
      return res + upRight();
   }

   std::wstring downBorder (int length)
   {
      std::wstring res = L"";
      res += downLeft();
      for (int i = 1; i < length - 1; ++i)
         res += horisontal();
      return res + downRight();
   }
}

