
#ifndef ARITHMETIC_DECODER_H
#define ARITHMETIC_DECODER_H

#include <math.h>

#include "common.h"

#ifdef WINDOWS
#include "file_map.h"
#else
#include "u_file_map.h"
#endif

#include "model.h"

//#pragma warning(push, 4)

////////////////////////////////////////////////////////

class arithmetic_decoder
{
private:
  file_map<bit>* m_input;
  const DWORD top_value;
  DWORD l, u, tag;

  arithmetic_decoder& operator=(const arithmetic_decoder&);
  arithmetic_decoder(const arithmetic_decoder& );

public: 
  arithmetic_decoder(file_map<bit>* fm): top_value(0xFFFFFFFF), 
                                         l(0), 
                                         u(top_value), 
                                         tag(0)
  {  
    m_input = fm;
    for (size_t i = 0; i < (sizeof(tag) * 8); ++i)
    {
      tag <<= 1;
      tag += m_input->read_next();
    }
  }  

  inline void decode(DWORD lower_bound, DWORD upper_bound, DWORD total)
  {
    bool eql_cond, e3_cond;
    QWORD range = (QWORD)u - l + 1;

    ASSERT(l < u);   
    ASSERT((lower_bound <= upper_bound) && (upper_bound <= total));
    
    u = l + (DWORD)(((range * upper_bound) / total) - 1);
    l = l + (DWORD)((range * lower_bound) / total);

    while( (eql_cond = (MSB(u) == MSB(l))) ||
           (e3_cond  = ((M2SB(l) == 1) && (M2SB(u) == 0))) )
    {  
      if(eql_cond)
      {
        l <<= 1;
        u <<= 1;
        tag <<= 1;
        
        u += 1;
        
        bit b = m_input->read_next();        
        ASSERT (((tag + b)  == tag + 1) || ((tag + b) == tag));        
        tag += b;
      }
      else if (e3_cond)
      {
        l <<= 1;
        u <<= 1;
        tag <<= 1;
        u += 1;
        
        bit b = m_input->read_next();
        ASSERT (((tag + b) == tag + 1) || ((tag + b) == tag));
        tag += b;
        
        l += MSB_MASK; 
        u += MSB_MASK;
        tag += MSB_MASK;
      }
    }
    ASSERT(l < u);
  }

  inline DWORD decode_target(DWORD total)
  {
    DWORD ret =  (DWORD)((((QWORD)(tag - l) + 1) * total - 1) / ((QWORD)u - l + 1));
    return (ret);
  }
};

#endif //ARITHMETIC_DECODER_H

////////////////////////////////////////////////////////

