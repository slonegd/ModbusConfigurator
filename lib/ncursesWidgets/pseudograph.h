#pragma once

#include <string>
#include <wchar.h>

namespace PSEUDO {
   constexpr wchar_t horisontal()   { return L'\x2501'; }
   constexpr wchar_t vertical()     { return L'\x2503'; }
   constexpr wchar_t upLeft()       { return L'\x250F'; }
   constexpr wchar_t upRight()      { return L'\x2513'; }
   constexpr wchar_t downLeft()     { return L'\x2517'; }
   constexpr wchar_t downRight()    { return L'\x251B'; }
   constexpr wchar_t upWithTap()    { return L'\x2533'; }
   constexpr wchar_t downWithTap()  { return L'\x253B'; }
   constexpr wchar_t leftWithTap()  { return L'\x2523'; }
   constexpr wchar_t rightWithTap() { return L'\x252B'; }
   constexpr wchar_t crossroad()    { return L'\x254B'; }

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

   std::wstring line (int length, wchar_t ch)
   {
      std::wstring res = L"";
      for (int i = 0; i < length; ++i)
         res += ch;
      return res;
   }
}

