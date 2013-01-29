
#ifndef PPM_MODEL_H
#define PPM_MODEL_H

#include <map>
#include <list>
#include <deque>
#include <string>

using namespace std;

#include "common.h"
#include "model.h"
#include "arithmetic_encoder.h"
#include "arithmetic_decoder.h"
#include "index.h"

const char escape = '\0';

class context_entry
{
public:
  char cLetter;
  DWORD dwCount;
  DWORD dwCumCount;
};

class context_entry_list
{
private:
  list<context_entry> m_List;
  context_entry m_ceEscape;
public:
  typedef list<context_entry>::iterator iterator;

  context_entry_list()
  {
    m_ceEscape.cLetter = escape;
    m_ceEscape.dwCount = 1;
    m_ceEscape.dwCumCount = 1;
  }

  inline DWORD total_count()
  {
    return(m_ceEscape.dwCumCount);
  }

  void insert(char cNewLetter)
  {
    context_entry ceNew;
    ceNew.cLetter = cNewLetter;
    ceNew.dwCount = 1;
    ceNew.dwCumCount = m_ceEscape.dwCumCount - m_ceEscape.dwCount + 1;

    m_List.push_back(ceNew);

    m_ceEscape.dwCount += 1;
    m_ceEscape.dwCumCount += 2;
  }

  void update(iterator i)
  {
    i->dwCount += 1;
    for (; i != end(); ++i)
    {
      i->dwCumCount += 1;
    }

    m_ceEscape.dwCumCount += 1;
  }

  iterator begin()
  {
    return(m_List.begin());
  }

  iterator end()
  {
    return(m_List.end());
  }

  inline DWORD escape_lower_bound()
  {
    return(m_ceEscape.dwCumCount - m_ceEscape.dwCount);
  }

  inline DWORD escape_upper_bound()
  {
    return(m_ceEscape.dwCumCount);
  }

  inline context_entry& escape_entry()
  {
    return(m_ceEscape);
  }
};

template<size_t S = 5>
class ppm_model : public model<char>
{
protected:
  typedef map<string, context_entry_list> context_order;

  context_order aContextOrders[S + 1];
  string sHistory;
  DWORD m_dwNegativeContextCumCount;

  arithmetic_encoder* m_encoder;
  arithmetic_decoder* m_decoder;

  ppm_model(const ppm_model&);
  ppm_model& operator=(const ppm_model&);

  context_entry search_symbol(DWORD dwTarget, context_entry_list& ContextEntryList);
  char rec_decode(string sContext, int iContext);

public:
  ppm_model(arithmetic_encoder* encoder): m_encoder(encoder),
                                          m_decoder(NULL) 
  {
  }

  ppm_model(arithmetic_decoder* decoder): m_encoder(NULL),
                                          m_decoder(decoder)
  {
  }

  virtual void encode_symbol(char cNewSymbol);
  virtual void encode_eof();
  virtual char decode_symbol();

  virtual ~ppm_model() 
  {
  }
};

///////////////////////////////////////////////////////////

template<size_t S>
context_entry ppm_model<S>::search_symbol(DWORD dwTarget, context_entry_list& ContextEntryList)
{
  context_entry ceRet;
    
  if ((ContextEntryList.escape_lower_bound  <= dwTarget) && 
      (dwTarget < ContextEntryList.escape_upper_bound))
  return ContextEntryList.escape_entry();
   
  DWORD dwPrevCumCount = 0;

  for(context_entry_list::iterator i = ContextEntryList.begin();
      i != ContextEntryList.end();
      ++i)
  {
    if ((dwPrevCumCount <= dwTarget) && (dwTarget < i->dwCumCount))
      return (*i);
  
    dwPrevCumCount = i->dwCumCount;        
  }
  return ceRet;
}

template<size_t S>
void ppm_model<S>::encode_symbol(char cNewSymbol)
{
  if (m_encoder)
  {
    bool bContextFound = false;
    int j = 0;
    
    for ( int iContext = sHistory.length();
         (iContext >= 0) && !bContextFound;
         --iContext)
    {   
      string sContext(sHistory, j, iContext);
      ++j;
      
      context_entry_list& ContextEntryList = aContextOrders[iContext][sContext];

      for(context_entry_list::iterator i = ContextEntryList.begin();
          (i != ContextEntryList.end()) && !bContextFound;
          ++i)
      {
        if(i->cLetter == cNewSymbol)
        {
          //codifica il simbolo
          m_encoder->encode(i->dwCumCount - i->dwCount,
                            i->dwCumCount,
                            ContextEntryList.total_count());
          
          //update (existing)
          ContextEntryList.update(i);
          
          bContextFound = true;
        }
      }

      if(!bContextFound)
      {
        //codifica un escape
        m_encoder->encode(ContextEntryList.escape_lower_bound(),
                          ContextEntryList.escape_upper_bound(),
                          ContextEntryList.total_count());
        //update_new
        ContextEntryList.insert(cNewSymbol);
      }
    }

    if(!bContextFound)
    {
      //-1 context order
      m_encoder->encode((BYTE)cNewSymbol - 1,
                        (BYTE)cNewSymbol,
                        0xFF);
    }

    if (sHistory.size() < S)
      sHistory += cNewSymbol;
    else
      sHistory = sHistory.substr(1) + cNewSymbol;
  }
}

template<size_t S>
void ppm_model<S>::encode_eof()
{
  m_encoder->encode_eof();
}

template<size_t S>
char ppm_model<S>::rec_decode(string sContext, int iContext)
{
  context_entry ceSymbol;
  context_entry_list::iterator i;

  if (iContext >= 0)
  {
    context_entry_list& ContextEntryList = aContextOrders[iContext][sContext];

    DWORD dwTarget = m_decoder->decode_target(ContextEntryList.total_count());    
      
    //search symbol       
    if ((ContextEntryList.escape_lower_bound()  <= dwTarget) && 
        (dwTarget < ContextEntryList.escape_upper_bound()))
    {
      ceSymbol= ContextEntryList.escape_entry();
    }
    else
    {
      DWORD dwPrevCumCount = 0;

      for(i = ContextEntryList.begin();
          i != ContextEntryList.end();
          ++i)
      {
        if ((dwPrevCumCount <= dwTarget) && (dwTarget < i->dwCumCount))
        {
          ceSymbol = *i;
          break;
        }      
        dwPrevCumCount = i->dwCumCount;
      }
    }
    //fine ricerca simbolo

    m_decoder->decode(ceSymbol.dwCumCount - ceSymbol.dwCount,
                      ceSymbol.dwCumCount,
                      ContextEntryList.total_count());
       
    if (ceSymbol.cLetter != escape)
    {
      ContextEntryList.update(i);
      return (ceSymbol.cLetter);
    }
    else
    {
      string sNewContext;
      
      if (sContext.size() == 0)
        sNewContext = sContext;
      else
        sNewContext = string(sContext, 1, iContext - 1);

      char cSymbol = rec_decode(sNewContext, iContext - 1);

      ASSERT(cSymbol != escape);
      
      ContextEntryList.insert(cSymbol);
      return (cSymbol);
    }
  }
  else 
  {
    //-1 context order
    
    DWORD dwTarget = m_decoder->decode_target((DWORD)0xFF);      
    char cSymbol = (char)(dwTarget + 1);

    m_decoder->decode((BYTE)cSymbol - 1,
                      (BYTE)cSymbol,
                      0xFF);
    return (cSymbol);
  }
}
    
template<size_t S>
char ppm_model<S>::decode_symbol()
{
  char cSymbol;

  if (m_decoder)
  {  
    cSymbol = rec_decode(sHistory, sHistory.length());
    
    if (sHistory.size() < S)
      sHistory += cSymbol;
    else
      sHistory = sHistory.substr(1) + cSymbol;
  }
  return cSymbol;
}

#endif //PPM_MODEL_H

