
#ifndef INDEX_H__
#define INDEX_H__

template<class T>
class idx
{
public:
  static inline DWORD max()
  {
    return ((1 << (sizeof(T) * 8)) - 1);
  }
  
  static inline size_t from_symbol(T symbol)
  {
    return ((size_t)symbol + 1);
    //return ((symbol) - '0');
  }
  
  static inline T to_symbol(DWORD index)
  {
    //return ((char)index + '0');
    return ((T)index - 1 );
  }
};

template<> 
class idx<bit>
{
public:
  static inline DWORD max()
  {
    return (2);
  }
  
  static inline size_t from_symbol(bit symbol)
  {
    return (symbol ? 2 : 1);
  }
  
  static inline bit to_symbol(DWORD index)
  {
    return ((index == 0) ? 0 : 1);
  }
};

#endif //INDEX_H__

