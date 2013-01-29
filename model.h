#ifndef MODEL_H
#define MODEL_H

#include "common.h"

template <typename symbolT>
class model
{
public:
  virtual void encode_symbol(symbolT symbol) = 0;
  virtual symbolT decode_symbol() = 0;
};

#endif
