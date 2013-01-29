
#ifndef JPEG_LS_MODEL_H
#define JPEG_LS_MODEL_H

#include <vector>

using namespace std;

#include "common.h"
#include "model.h"
#include "arithmetic_encoder.h"
#include "arithmetic_decoder.h"
#include "index.h"
#include "adaptive_model.h"
#include "bitmap.h"
#include <algorithm>

using namespace std;

class jpeg_ls_model : public adaptive_model<BYTE>
{
private:
  BYTE m_PredictedValue;
  bitmap8bpp* m_Bmp;

  jpeg_ls_model(const jpeg_ls_model&);
  jpeg_ls_model& operator=(const jpeg_ls_model&);

public:
  jpeg_ls_model(arithmetic_encoder* encoder, bitmap8bpp* bmp);
  jpeg_ls_model(arithmetic_decoder* decoder, bitmap8bpp* bmp);

  void predict5(DWORD i, DWORD j);

  virtual void encode_symbol(BYTE symbol);
  virtual void encode_eof();
  virtual BYTE decode_symbol();
  virtual ~jpeg_ls_model() 
  {
  }
};

///////////////////////////////////////////////////////////

jpeg_ls_model::jpeg_ls_model(arithmetic_encoder* encoder, bitmap8bpp* bmp) : 
  adaptive_model<BYTE>(encoder), m_Bmp(bmp)
{
  m_PredictedValue = 0;
}

jpeg_ls_model::jpeg_ls_model(arithmetic_decoder* decoder, bitmap8bpp* bmp) : 
  adaptive_model<BYTE>(decoder), m_Bmp(bmp)
{
  m_PredictedValue = 0;
}

void jpeg_ls_model::predict5(DWORD i, DWORD j)
{
  BYTE N =  m_Bmp->get_greyed_pixel(i - 1, j);
  BYTE W =  m_Bmp->get_greyed_pixel(i, j - 1);
  BYTE NW = m_Bmp->get_greyed_pixel(i - 1, j - 1);
  
  if (!(NW < _MAX(W, N)))
  {
    m_PredictedValue = _MAX(W, N);
  }
  else
  {
    if (!(NW > _MIN(W, N)))
      m_PredictedValue = _MIN(W, N);
    else
      m_PredictedValue = W + N - NW;
  }
}

void jpeg_ls_model::encode_symbol(BYTE symbol)
{
  if (m_encoder)
  {
    BYTE cbDistance = symbol - m_PredictedValue;
    DWORD iDistance = idx<BYTE>::from_symbol(cbDistance);

    m_encoder->encode(cum_count(iDistance - 1),
                      cum_count(iDistance), 
                      total_count());

    update(cbDistance);
  }
}

void jpeg_ls_model::encode_eof()
{
  m_encoder->encode_eof();
}

BYTE jpeg_ls_model::decode_symbol()
{
  BYTE symbol;
  if (m_decoder)
  {  
    DWORD target = m_decoder->decode_target(total_count());    
    DWORD index = search_index(target);

    m_decoder->decode(cum_count(index - 1),
                      cum_count(index),
                      total_count());

    BYTE cbDistance = idx<BYTE>::to_symbol(index);
    symbol = m_PredictedValue + cbDistance;
    update(cbDistance);
  }

  return symbol;
}

//////////////////////////// The model for true color BMPs ///////////////////////////////////

class jpeg_ls_24_model : public model<rgb_color24>
{
private:
  rgb_color24 m_rgbPredictedValue;

  bitmap_true<rgb_color24>* m_Bmp;

  enum { dwSize = 257 };

  DWORD m_aRedCumCount  [dwSize];
  DWORD m_aGreenCumCount[dwSize];
  DWORD m_aBlueCumCount [dwSize];
  //DWORD m_aAlphaCumCount[dwSize];
  
  arithmetic_encoder* m_encoder;
  arithmetic_decoder* m_decoder;

  jpeg_ls_24_model(const jpeg_ls_24_model&);
  jpeg_ls_24_model& operator=(const jpeg_ls_24_model&);

  void  init(DWORD aColorCumCount[]);

  DWORD cum_count(size_t iSymbol, DWORD aColorCumCount[]);
  DWORD search_index(DWORD dwTarget, DWORD aColorCumCount[]);
  DWORD total_count(DWORD aColorCumCount[]);

  void update(BYTE bColorChannel, DWORD aColorCumCount[]);
  void update(rgb_color24 rgbNewColor);

  void encode_channel(DWORD iColorDistance, DWORD aColorCumCount[]);

public:
  jpeg_ls_24_model(arithmetic_encoder* encoder, bitmap_true<rgb_color24>* bmp);
  jpeg_ls_24_model(arithmetic_decoder* decoder, bitmap_true<rgb_color24>* bmp);

  void predict5(DWORD i, DWORD j);

  virtual void encode_symbol(rgb_color24 symbol);
  virtual void encode_eof();
  virtual rgb_color24 decode_symbol();
  virtual ~jpeg_ls_24_model() 
  {
  }
};

////////////////////////////// ctor /////////////////////////////

jpeg_ls_24_model::jpeg_ls_24_model(arithmetic_encoder* encoder, bitmap_true<rgb_color24>* bmp) : 
  m_Bmp(bmp),
  m_encoder(encoder),
  m_decoder(NULL)
{
  init(m_aRedCumCount);
  init(m_aGreenCumCount);
  init(m_aBlueCumCount);
  //init(m_aAlphaCumCount);
}

jpeg_ls_24_model::jpeg_ls_24_model(arithmetic_decoder* decoder, bitmap_true<rgb_color24>* bmp) : 
  m_Bmp(bmp),
  m_encoder(NULL),
  m_decoder(decoder)
{
  init(m_aRedCumCount);
  init(m_aGreenCumCount);
  init(m_aBlueCumCount);
  //init(m_aAlphaCumCount);
}

////////////////////////// private //////////////////////////////

void jpeg_ls_24_model::init(DWORD aColorCumCount[])
{
  aColorCumCount[0] = 0;
  for (DWORD i = 1; i < dwSize; ++i)
    aColorCumCount[i] = i;
}

inline DWORD jpeg_ls_24_model::cum_count(size_t iSymbol, DWORD aColorCumCount[])
{
  return(aColorCumCount[iSymbol]);
}

inline DWORD jpeg_ls_24_model::search_index(DWORD dwTarget, DWORD aColorCumCount[])
{
  register DWORD k;
  for (k = 1; k < dwSize; ++k)
    if ((aColorCumCount[k - 1] <= dwTarget) && (dwTarget < aColorCumCount[k]))
      break;
  return k;
}

inline DWORD jpeg_ls_24_model::total_count(DWORD aColorCumCount[])
{
  ASSERT(aColorCumCount[dwSize - 1] > aColorCumCount[dwSize - 2]);
  return(aColorCumCount[dwSize - 1]);
}

void jpeg_ls_24_model::update(BYTE bColorChannel, DWORD aColorCumCount[])
{
  for (DWORD j = idx<BYTE>::from_symbol(bColorChannel); j < dwSize; ++j)
  {
    ++(aColorCumCount[j]);
  } 

  if (aColorCumCount[dwSize - 1] >= ((DWORD)UPPER_LIMIT >> 2) )
  {
    for (DWORD i = 1; i < dwSize; ++i)
    {
      aColorCumCount[i] >>= 1;
      while (aColorCumCount[i] <= aColorCumCount[i - 1])
        ++aColorCumCount[i];
    }
  }
} 
 
void jpeg_ls_24_model::update(rgb_color24 rgbNewColor)
{
  update(rgbNewColor.rgbRed,      m_aRedCumCount  );
  update(rgbNewColor.rgbGreen,    m_aGreenCumCount);
  update(rgbNewColor.rgbBlue,     m_aBlueCumCount );
  //update(rgbNewColor.rgbReserved, m_aAlphaCumCount);
}

void jpeg_ls_24_model::predict5(DWORD i, DWORD j)
{
  rgb_color24 rgbN = m_Bmp->get_pixel(i - 1, j);
  rgb_color24 rgbW = m_Bmp->get_pixel(i, j - 1);
  rgb_color24 rgbNW = m_Bmp->get_pixel(i - 1, j - 1);
  
  if (!(rgbNW < _MAX(rgbW, rgbN)))
  {
    m_rgbPredictedValue = _MAX(rgbW, rgbN);
  }
  else
  {
    if (!(rgbNW > _MIN(rgbW, rgbN)))
      m_rgbPredictedValue = _MIN(rgbW, rgbN);
    else
      m_rgbPredictedValue = rgbW + rgbN - rgbNW;
  }
}

void jpeg_ls_24_model::encode_channel(DWORD iColorDistance, DWORD aColorCumCount[])
{
  m_encoder->encode(cum_count(iColorDistance - 1, aColorCumCount),
                    cum_count(iColorDistance, aColorCumCount), 
                    total_count(aColorCumCount));
}

/////////////////////////// publics ////////////////////////////////////

void jpeg_ls_24_model::encode_symbol(rgb_color24 rgbSymbol)
{
  if (m_encoder)
  {
    DWORD iDistance;
    
    rgb_color24 rgbDistance = rgbSymbol - m_rgbPredictedValue;

    iDistance = idx<BYTE>::from_symbol(rgbDistance.rgbRed);
    encode_channel(iDistance, m_aRedCumCount);

    iDistance = idx<BYTE>::from_symbol(rgbDistance.rgbGreen);
    encode_channel(iDistance, m_aGreenCumCount);
    
    iDistance = idx<BYTE>::from_symbol(rgbDistance.rgbBlue);
    encode_channel(iDistance, m_aBlueCumCount);

    //iDistance = idx<BYTE>::from_symbol(rgbDistance.rgbReserved);
    //encode_channel(iDistance, m_aAlphaCumCount);

    update(rgbDistance);
  }
}

void jpeg_ls_24_model::encode_eof()
{
  m_encoder->encode_eof();
}

rgb_color24 jpeg_ls_24_model::decode_symbol()
{
  rgb_color24 rgbSymbol;
  if (m_decoder)
  {  
    rgb_color24 rgbDistance;
    
    //red
    DWORD dwRedTarget = m_decoder->decode_target(total_count(m_aRedCumCount));
    DWORD iRed = search_index(dwRedTarget, m_aRedCumCount);

    m_decoder->decode(cum_count(iRed - 1, m_aRedCumCount),
                      cum_count(iRed, m_aRedCumCount),
                      total_count(m_aRedCumCount));

    BYTE cbRedDistance = idx<BYTE>::to_symbol(iRed);
    rgbDistance.rgbRed = cbRedDistance;

    //green
    DWORD dwGreenTarget = m_decoder->decode_target(total_count(m_aGreenCumCount));
    DWORD iGreen = search_index(dwGreenTarget, m_aGreenCumCount);

    m_decoder->decode(cum_count(iGreen - 1, m_aGreenCumCount),
                      cum_count(iGreen, m_aGreenCumCount),
                      total_count(m_aGreenCumCount));

    BYTE cbGreenDistance = idx<BYTE>::to_symbol(iGreen);
    rgbDistance.rgbGreen = cbGreenDistance;
    
    //blue
    DWORD dwBlueTarget = m_decoder->decode_target(total_count(m_aBlueCumCount));
    DWORD iBlue = search_index(dwBlueTarget, m_aBlueCumCount);

    m_decoder->decode(cum_count(iBlue - 1, m_aBlueCumCount),
                      cum_count(iBlue, m_aBlueCumCount),
                      total_count(m_aBlueCumCount));

    BYTE cbBlueDistance = idx<BYTE>::to_symbol(iBlue);
    rgbDistance.rgbBlue = cbBlueDistance;
    
    //alpha
    /*DWORD dwAlphaTarget = m_decoder->decode_target(total_count(m_aAlphaCumCount));
    DWORD iAlpha = search_index(dwAlphaTarget, m_aAlphaCumCount);

    m_decoder->decode(cum_count(iAlpha - 1, m_aAlphaCumCount),
                      cum_count(iAlpha, m_aAlphaCumCount),
                      total_count(m_aAlphaCumCount));

    BYTE cbAlphaDistance = idx<BYTE>::to_symbol(iAlpha);
    rgbDistance.rgbReserved = cbAlphaDistance;*/

    rgbSymbol = m_rgbPredictedValue + rgbDistance;
    update(rgbDistance);
  }

  return rgbSymbol;
}

#endif //JPEG_LS_MODEL_H

