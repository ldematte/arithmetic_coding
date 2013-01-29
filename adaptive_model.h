
#ifndef ADAPTIVE_MODEL_H
#define ADAPTIVE_MODEL_H

#include <vector>

using namespace std;

#include "common.h"
#include "model.h"
#include "arithmetic_encoder.h"
#include "arithmetic_decoder.h"
#include "index.h"

template <typename symbolT>
class adaptive_model : public model<symbolT>
{
protected:
  DWORD dwSize;  
  vector<DWORD> m_cum_count;
  
  arithmetic_encoder* m_encoder;
  arithmetic_decoder* m_decoder;

  adaptive_model(const adaptive_model<symbolT>&);
  adaptive_model<symbolT>& operator=(const adaptive_model<symbolT>&);

  void update(symbolT new_symbol);

  inline DWORD cum_count(size_t iSymbol)
  {
    return(m_cum_count[iSymbol]);
  }

  inline DWORD search_index(DWORD target)
  {
    register DWORD k;
    for (k = 1; k < dwSize; ++k)
      if ((m_cum_count[k - 1] <= target) && (target <m_cum_count[k]))
        break;
    return k;
  }

  inline DWORD total_count()
  {
    ASSERT(m_cum_count[dwSize - 1] > m_cum_count[dwSize - 2]);
    return(m_cum_count[dwSize - 1]);
  }
  
  inline void init()
  {
    m_cum_count[0] = 0;
    for (DWORD i = 1; i < dwSize; ++i)
      m_cum_count[i] = i;
  }

public:
  adaptive_model(arithmetic_encoder* encoder);
  adaptive_model(arithmetic_decoder* decoder);

  virtual void encode_symbol(symbolT symbol);
  virtual void encode_eof();
  virtual symbolT decode_symbol();
  virtual ~adaptive_model() {}
};

///////////////////////////////////////////////////////////

template<typename symbolT>
adaptive_model<symbolT>::adaptive_model(arithmetic_encoder* encoder) : 

  dwSize((idx<symbolT>::max()) + 2/*4*/),
  m_cum_count(dwSize),
  m_encoder(encoder),
  m_decoder(NULL)

{
  init();
}

template<typename symbolT>
adaptive_model<symbolT>::adaptive_model(arithmetic_decoder* decoder) : 

  dwSize(idx<symbolT>::max() + 2/*4*/), 
  m_cum_count(dwSize),
  m_encoder(NULL),
  m_decoder(decoder)

{
  init();
}

template<typename symbolT>
void adaptive_model<symbolT>::update(symbolT new_symbol)
{
  for (DWORD j = idx<symbolT>::from_symbol(new_symbol); j < dwSize; ++j)
  {
    ++(m_cum_count[j]);
    
  } 

  if (m_cum_count[dwSize - 1] >= ((DWORD)UPPER_LIMIT >> 2) )
  {
    for (DWORD i = 1; i < dwSize; ++i)
    {
      m_cum_count[i] >>= 1;
      while (m_cum_count[i] <= m_cum_count[i - 1])
        ++m_cum_count[i];
    }
  }
}

template<typename symbolT>
void adaptive_model<symbolT>::encode_symbol(symbolT symbol)
{
  if (m_encoder)
  {
    m_encoder->encode(cum_count(idx<symbolT>::from_symbol(symbol) - 1),
                      cum_count(idx<symbolT>::from_symbol(symbol)), //symbol_to_index(symbol)),
                      total_count());
    update(symbol);    
  }
}

template<typename symbolT>
void adaptive_model<symbolT>::encode_eof()
{
  m_encoder->encode_eof();
}

template<typename symbolT>
symbolT adaptive_model<symbolT>::decode_symbol()
{
  symbolT symbol;
  if (m_decoder)
  {  
    DWORD target = m_decoder->decode_target(total_count());    
    DWORD index = search_index(target);

    m_decoder->decode(cum_count(index - 1),
                      cum_count(index),
                      total_count());

    symbol = idx<symbolT>::to_symbol(index);
    update(symbol);
  }

  return symbol;
}

#endif

