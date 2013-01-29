
#ifndef DIFF_ADAPTIVE_MODEL_H
#define DIFF_ADAPTIVE_MODEL_H

#include <vector>

using namespace std;

#include "common.h"
#include "model.h"
#include "arithmetic_encoder.h"
#include "arithmetic_decoder.h"
#include "index.h"
#include "adaptive_model.h"

template <typename symbolT>
class difference_adaptive_model : public adaptive_model<symbolT>
{
private:
  symbolT m_tLastSymbol;

  difference_adaptive_model(const difference_adaptive_model<symbolT>&);
  difference_adaptive_model<symbolT>& operator=(const difference_adaptive_model<symbolT>&);
  
  inline void init()
  {
    adaptive_model<symbolT>::init();
    m_tLastSymbol = 0;
  }

public:
  difference_adaptive_model(arithmetic_encoder* encoder);
  difference_adaptive_model(arithmetic_decoder* decoder);

  virtual void encode_symbol(symbolT& symbol);
  virtual void encode_eof();
  virtual symbolT decode_symbol();
  virtual ~difference_adaptive_model() 
  {
  }
};

///////////////////////////////////////////////////////////

template<typename symbolT>
difference_adaptive_model<symbolT>::difference_adaptive_model(arithmetic_encoder* encoder) : 
  adaptive_model<symbolT>(encoder)
{
  init();
}

template<typename symbolT>
difference_adaptive_model<symbolT>::difference_adaptive_model(arithmetic_decoder* decoder) : 
  adaptive_model<symbolT>(decoder)
{
  init();
}

template<typename symbolT>
void difference_adaptive_model<symbolT>::encode_symbol(symbolT& symbol)
{
  if (m_encoder)
  {
    symbolT tDistance = symbol - m_tLastSymbol;
    DWORD iDistance = idx<symbolT>::from_symbol(tDistance);

     m_encoder->encode(cum_count(iDistance - 1),
                      cum_count(iDistance), //symbol_to_index(symbol)),
                      total_count());

    m_tLastSymbol = symbol;

    update(tDistance);
  }
}

template<typename symbolT>
void difference_adaptive_model<symbolT>::encode_eof()
{
  m_encoder->encode_eof();
}

template<typename symbolT>
symbolT difference_adaptive_model<symbolT>::decode_symbol()
{
  symbolT symbol;
  if (m_decoder)
  {  
    DWORD target = m_decoder->decode_target(total_count());    
    DWORD index = search_index(target);

    m_decoder->decode(cum_count(index - 1),
                      cum_count(index),
                      total_count());

    symbolT tDistance = idx<symbolT>::to_symbol(index);
    symbol = m_tLastSymbol + tDistance;
    m_tLastSymbol = symbol;
    update(tDistance);
  }

  return symbol;
}

#endif

