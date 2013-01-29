
#ifndef ARITHMETIC_ENCODER_H
#define ARITHMETIC_ENCODER_H

#include <math.h>

#include "common.h"
#ifdef WINDOWS
#include "file_writer.h"
#else
#include "u_file_writer.h"
#endif

////////////////////////////////////////////////////////

class arithmetic_encoder
{
private:
  
  file_writer<bit>* m_output;
  const DWORD top_value;
  DWORD l, u;
  int scale3;

  arithmetic_encoder& operator=(const arithmetic_encoder&);
  arithmetic_encoder(const arithmetic_encoder& );

public: 
  
  arithmetic_encoder(file_writer<bit>* fw) : top_value(0xFFFFFFFF), 
                                             l(0), 
                                             u(top_value), 
                                             scale3(0)
  {
    m_output = fw;
  }
    
  inline void encode(DWORD lower_bound, DWORD upper_bound, DWORD total)
  {    
    bool e3_cond = false;
    bool eql_cond;

    QWORD range = (QWORD)(u - l) + 1;

    ASSERT(l < u);
    ASSERT((lower_bound <= upper_bound) && (upper_bound <= total));
    
    u = l + (DWORD)(((range * upper_bound) / total) - 1);
    l = l + (DWORD)((range * lower_bound) / total);

    while( (eql_cond = (MSB(u) == MSB(l))) ||
           (e3_cond  = (M2SB(l) == 1) && (M2SB(u) == 0)) )
    {  
      if(eql_cond)
      {
        bit bit_to_send = ((MSB(u) == 0) ? 0 : 1);
        m_output->send(bit_to_send);
        
        l <<= 1;
        u <<= 1;
        u |= 1;

        while (scale3 > 0)
        {
          m_output->send(!bit_to_send);
          --scale3;
        }
      }
      else if (e3_cond)
      {
        l <<= 1;
        u <<= 1;
        u |= 1;
        
        l += MSB_MASK; 
        u += MSB_MASK;

        ++scale3;
      }
    }
    ASSERT(l < u);
  }

  inline void encode_eof()
  {
    for (size_t i = 0; i < sizeof(DWORD) * 8; ++i)
    {
      bit bit_to_send = ((MSB(l) == 0) ? 0 : 1);
      m_output->send(bit_to_send);
        
      l <<= 1;
      while (scale3 > 0)
      {
        m_output->send(!bit_to_send);
        --scale3;
      }
    }
  }
};

////////////////////////////////////////////////////////

#endif //ARITHMETIC_ENCODER_H                 

