
#ifndef BIT_FILE_MAP_H
#define BIT_FILE_MAP_H

#ifdef WINDOWS
#include <windows.h>
#include "file_map.h"
#include "file_writer.h"
#else
#include "u_file_map.h"
#include "u_file_writer.h"
#include "bmpDEF.h"
#endif

#include "rgb_color.h"

//per gli algoritmi di sort
#include <algorithm>

using namespace std;

class wrong_bpp {  };

#pragma pack(1)
class bitmap_header: public BITMAPFILEHEADER, public BITMAPINFOHEADER
{
public:
  inline DWORD height()
  {
    return (biHeight);
  }

  inline DWORD width()
  {
    return (biWidth);
  }

  inline int get_bpp()
  {
    return (biBitCount);
  }
  
  inline int get_palette_size()
  {
    if (biBitCount >= 16) 
      return 0;
    return (biClrUsed);
  }

  inline int get_struct_size()
  {
    return (biSize + sizeof(BITMAPFILEHEADER));
  }

  inline int get_header_size()
  {
    return (bfOffBits);
  }
  
  inline DWORD get_image_size()
  {
    return (biHeight * biWidth * biBitCount / 8);
  }

  inline DWORD get_file_size()
  {
    return (bfSize);
  }

  inline DWORD get_compresion_flag()
  {
    return (biCompression);
  }

  inline void set_compression_flag(DWORD dwFlag)
  {
    biCompression = dwFlag;
  }
};
#pragma pack()

class bitmap8bpp: private file_map<BYTE>
{
private:
  
  /////////////////////////// Dichiarazione classi ausiliarie ////////////////////////////////
  
  struct indexed_rgb
  {
    rgb_color32 rgbValue;
    BYTE iValue;
  };  
  
  class compare_indexed_rgb
  {
  public:
    int operator() (const indexed_rgb& a, const indexed_rgb& b)
    {
      return (a.rgbValue.to_dword() < b.rgbValue.to_dword());
    }
  };

  //////////////////////////////// Variabili private ////////////////////////////////////////
  
  bitmap_header* m_pBitmapHeader;
 
  rgb_color32* m_aRgb;
  BYTE*        m_aColorIndex;
  
  indexed_rgb m_IndexedPalette[256];
  int         m_IndexToGrey[256];  
  
  ////////////////////////////////// Metodi privati ////////////////////////////////////////
  
  void translate_simil_grey()
  {
    //create the rgb_color32 - index array    
    for (int i = 0; i < m_pBitmapHeader->get_palette_size(); ++i)
    {
      m_IndexedPalette[i].rgbValue = m_aRgb[i];
      m_IndexedPalette[i].iValue = i;
    }
    
    //ordina con shell-sort    
    stable_sort(m_IndexedPalette, 
                m_IndexedPalette + m_pBitmapHeader->get_palette_size(),
                            compare_indexed_rgb());
                
    for(int j = 0; j < m_pBitmapHeader->get_palette_size(); ++j)
    {
      m_IndexToGrey[m_IndexedPalette[j].iValue] = j;
    }
  }

  //////////////////////////////// Disbilitazione copia ////////////////////////////////////
  
  bitmap8bpp(const bitmap8bpp&);
  bitmap8bpp& operator=(const bitmap8bpp&);

public:
 
  bitmap8bpp(char* szFileName, DWORD dwFileSize = 0)
  {  
    if (!this->create(szFileName, dwFileSize))
      throw new bad_file();
    
    m_pBitmapHeader = (bitmap_header*)m_FileMap;
    
    if (m_pBitmapHeader->get_bpp() != 8)
      throw new wrong_bpp();
      
    m_aRgb = (rgb_color32*)(m_FileMap + m_pBitmapHeader->get_struct_size());    
    m_aColorIndex = (m_FileMap + m_pBitmapHeader->get_header_size());

    translate_simil_grey();
  }  

  inline DWORD height()
  {
    return(m_pBitmapHeader->height());
  }

  inline DWORD width()
  {
    return(m_pBitmapHeader->width());
  }

  inline BYTE get_greyed_pixel(DWORD x, DWORD y)
  {
    if ( (x > (height() - 1)) || (y > (width() - 1)) ) 
     return 0; //pixel fuori immagine

    return (m_IndexToGrey[m_aColorIndex[x * width() + y]]);
  }

  inline void set_greyed_pixel(DWORD x, DWORD y, BYTE pixel_value)
  {
    ASSERT((x <= (height() - 1)) && (y <= (width() - 1)));
    if ((x <= (height() - 1)) && (y <= (width() - 1))) 
      m_aColorIndex[x * width() + y] = m_IndexedPalette[pixel_value].iValue;
  }
  
  inline rgb_color32 get_pixel(DWORD x, DWORD y)
  {    
    if ( (x > (height() - 1)) || (y > (width() - 1)) ) 
    {
      //pixel fuori immagine
      rgb_color32 ret;
      return (ret);
    }
    return (m_aRgb[m_aColorIndex[x * width() + y]]);
  }

  inline rgb_color32* get_palette()
  {
    return m_aRgb;
  }

  inline bitmap_header* get_header()
  {
    return m_pBitmapHeader;
  }
  
  inline int get_palette_size()
  {
    return (m_pBitmapHeader->get_palette_size());
  }
};

template<class RGB_TYPE>  
class bitmap_true: private file_map<BYTE>
{
private:
  
  bitmap_header* m_pBitmapHeader; 
  RGB_TYPE*     m_aColorIndex;

  ///////////////////////////// Disabilitazione copia //////////////////////////////

  bitmap_true(const bitmap_true<RGB_TYPE>&);
  bitmap_true<RGB_TYPE>& operator=(const bitmap_true<RGB_TYPE>&);

public: 
  
  bitmap_true(char* szFileName, DWORD dwFileSize = 0)
  { 
    if (!this->create(szFileName, dwFileSize))
      throw new bad_file();
    
    m_pBitmapHeader = (bitmap_header*)m_FileMap; 

    if (m_pBitmapHeader->get_bpp() != (sizeof(RGB_TYPE) * 8))
      throw new wrong_bpp();      
    
    if (m_pBitmapHeader->bfType != 0x4d42) // 0x42 = "B" 0x4d = "M" 
      throw new bad_file();

    m_aColorIndex = (RGB_TYPE*)(m_FileMap + m_pBitmapHeader->get_header_size());    
  }  

  inline DWORD height()
  {
    return(m_pBitmapHeader->height());
  }

  inline DWORD width()
  {
    return(m_pBitmapHeader->width());
  }

  inline RGB_TYPE get_pixel(DWORD x, DWORD y)
  {    
    if ( (x > (height() - 1)) || (y > (width() - 1)) ) 
    {
      //pixel fuori immagine
      RGB_TYPE ret;
      return (ret);
    }
    return (m_aColorIndex[x * width() + y]);
  }

  inline void set_pixel(DWORD x, DWORD y, RGB_TYPE rgbColor)
  {
    ASSERT ((x <= (height() - 1)) && (y <= (width() - 1)));
    if ( (x <= (height() - 1)) && (y <= (width() - 1)) ) 
      m_aColorIndex[x * width() + y] = rgbColor;
  }

  inline bitmap_header* get_header()
  {
    return m_pBitmapHeader;
  }
};

#endif //BIT_FILE_MAP_H

