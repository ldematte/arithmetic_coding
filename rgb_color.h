
#ifndef RGB_COLOR_CLASS_H__
#define RGB_COLOR_CLASS_H__

#ifdef WINDOWS
#include <windows.h>
#else
#include "bmpDEF.h"
#endif

class rgb_color32: public RGBQUAD
{
public:
  rgb_color32(BYTE cRed = 0, BYTE cGreen = 0, BYTE cBlue = 0, BYTE cAlpha = 0)
  {
    rgbRed = cRed;
    rgbGreen = cGreen;
    rgbBlue = cBlue;
    rgbReserved = cAlpha;
  }

  rgb_color32(RGBQUAD rgbNew)
  {
    rgbRed = rgbNew.rgbRed;
    rgbGreen = rgbNew.rgbGreen;
    rgbBlue = rgbNew.rgbBlue;
    rgbReserved = rgbNew.rgbReserved;
  }

  rgb_color32(DWORD dwRGB)
  {
    rgbBlue     = (BYTE)(dwRGB & 0xFF);
    rgbGreen    = (BYTE)((dwRGB >> 8) & 0xFF);
    rgbRed      = (BYTE)((dwRGB >> 16) & 0xFF);
    rgbReserved = (BYTE)((dwRGB >> 24) & 0xFF);    
  }

  inline DWORD to_dword() const
  {
    DWORD dwValue = (rgbReserved << 24) + 
                    (rgbRed      << 16) + 
                    (rgbGreen    << 8) + 
                     rgbBlue;
    return dwValue;
  }
};

inline rgb_color32 operator+(rgb_color32 a, rgb_color32 b)
{
  return rgb_color32(a.rgbRed + b.rgbRed,
                   a.rgbGreen + b.rgbGreen,
                   a.rgbBlue + b.rgbBlue,
                   a.rgbReserved + b.rgbReserved);
}

inline rgb_color32 operator-(rgb_color32 a, rgb_color32 b)
{
  return rgb_color32(a.rgbRed - b.rgbRed,
                   a.rgbGreen - b.rgbGreen,
                   a.rgbBlue - b.rgbBlue,
                   a.rgbReserved - b.rgbReserved);
}

inline rgb_color32 operator*(rgb_color32 a, int b)
{
  return rgb_color32(a.rgbRed * b,
                   a.rgbGreen * b,
                   a.rgbBlue * b,
                   a.rgbReserved * b);
}

inline rgb_color32 operator/(rgb_color32 a, int b)
{
  ASSERT(b != 0);
  return rgb_color32(a.rgbRed / b,
                   a.rgbGreen / b,
                   a.rgbBlue / b,
                   a.rgbReserved / b);
}

inline bool operator==(const rgb_color32 a, const rgb_color32 b)
{
  return (a.to_dword() == b.to_dword());
}

inline bool operator<(const rgb_color32 a, const rgb_color32 b)
{
  return (a.to_dword() < b.to_dword());
}

inline bool operator>(const rgb_color32 a, const rgb_color32 b)
{
  return (a.to_dword() > b.to_dword());
}

#pragma pack(1)
class rgb_color24
{
public:
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;

  rgb_color24(BYTE cRed = 0, BYTE cGreen = 0, BYTE cBlue = 0)
  {
    rgbRed = cRed;
    rgbGreen = cGreen;
    rgbBlue = cBlue;
  }

  rgb_color24(RGBTRIPLE rgbNew)
  {
    rgbRed = rgbNew.rgbtRed;
    rgbGreen = rgbNew.rgbtGreen;
    rgbBlue = rgbNew.rgbtBlue;
  }

  rgb_color24(DWORD dwRGB)
  {
    rgbBlue     = (BYTE)(dwRGB & 0xFF);
    rgbGreen    = (BYTE)((dwRGB >> 8) & 0xFF);
    rgbRed      = (BYTE)((dwRGB >> 16) & 0xFF);  
  }

  inline DWORD to_dword() const
  {
    DWORD dwValue = (rgbRed      << 16) + 
                    (rgbGreen    << 8) + 
                     rgbBlue;
    return dwValue;
  }

  inline operator RGBTRIPLE() const
  {
    RGBTRIPLE rgbRetVal = { rgbBlue, rgbGreen, rgbRed, };
    return rgbRetVal;
  }
};

inline rgb_color24 operator+(rgb_color24 a, rgb_color24 b)
{
  return rgb_color24(a.rgbRed + b.rgbRed,
                   a.rgbGreen + b.rgbGreen,
                   a.rgbBlue + b.rgbBlue);
}

inline rgb_color24 operator-(rgb_color24 a, rgb_color24 b)
{
  return rgb_color24(a.rgbRed - b.rgbRed,
                   a.rgbGreen - b.rgbGreen,
                   a.rgbBlue - b.rgbBlue);
}

inline rgb_color24 operator*(rgb_color24 a, int b)
{
  return rgb_color24(a.rgbRed * b,
                   a.rgbGreen * b,
                   a.rgbBlue * b);
}

inline rgb_color24 operator/(rgb_color24 a, int b)
{
  ASSERT(b != 0);
  return rgb_color24(a.rgbRed / b,
                   a.rgbGreen / b,
                   a.rgbBlue / b);
}

inline bool operator==(const rgb_color24 a, const rgb_color24 b)
{
  return (a.to_dword() == b.to_dword());
}

inline bool operator<(const rgb_color24 a, const rgb_color24 b)
{
  return (a.to_dword() < b.to_dword());
}

inline bool operator>(const rgb_color24 a, const rgb_color24 b)
{
  return (a.to_dword() > b.to_dword());
}

#pragma pack()

#endif //RGB_COLOR_CLASS_H__

