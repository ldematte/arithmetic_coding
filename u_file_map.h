
#ifndef FILE_MAP_H
#define FILE_MAP_H

#define _POSIX_MAPPED_FILE
 
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

  class bad_file { };

  template<class T>
  class file_map
  {
  protected:
    int m_fdFile;

    T* m_DataMap;
    BYTE* m_FileMap;

    DWORD m_dwFileSize; 
    DWORD m_iCurrentElem;
    DWORD m_dwSizeOfHeader;
    bool m_bEof;
    bool m_bHand;

    file_map<T>& operator=(const file_map<T>&);
    file_map(const file_map<T>&);

  public:

    file_map(DWORD dwSizeOfHeader = 0): m_fdFile(-1), 
                                        m_dwFileSize(0),
                                        m_iCurrentElem(0), 
                                        m_dwSizeOfHeader(dwSizeOfHeader),
                                        m_bEof(false)
    {
      
    }
     
    file_map(char* szFileName, DWORD dwSizeOfHeader = 0, DWORD dwFileSize = 0): 

                                m_fdFile(-1),
                                m_dwFileSize(dwFileSize), 
                                m_iCurrentElem(0),
                                m_dwSizeOfHeader(dwSizeOfHeader),
                                m_bEof(false)
    {
      if(!this->create(szFileName, dwFileSize))
        throw new bad_file();
    }

    bool create(char* szFileName, DWORD dwFileSize = 0)
    {
      m_fdFile = open(szFileName, O_RDWR);
      
      ASSERT(m_fdFile != -1);
      if (m_fdFile != -1)
      {
        struct stat stFile;     
        fstat(m_fdFile, &stFile);       
        
        if (dwFileSize == 0)
        {
          m_dwFileSize = stFile.st_size;          
          m_FileMap = (BYTE*)mmap(0, m_dwFileSize, PROT_READ | PROT_WRITE, 
                                  MAP_SHARED, m_fdFile, 0);                               
          m_bHand = false;
        }
        else
        {
          m_dwFileSize = dwFileSize;
          
          m_FileMap = (BYTE*)malloc(m_dwFileSize);
          
          ASSERT(m_FileMap != NULL);  
          if (m_FileMap == NULL)
            m_FileMap = (BYTE*)MAP_FAILED;
          
          /*m_FileMap = (BYTE*)mmap(0, m_dwFileSize, PROT_READ | PROT_WRITE, 
                                  MAP_ANON, -1, 0);*/
                                  
          read(m_fdFile, m_FileMap, stFile.st_size);
          m_bHand = true;
        }
        
        m_DataMap = (T*)(m_FileMap + m_dwSizeOfHeader);
          
        ASSERT(m_FileMap != MAP_FAILED);
        if (m_FileMap == MAP_FAILED)
        {
          throw new bad_alloc();
        }
        else
        {
          m_iCurrentElem = 0;          
          return true;
        }        
      }
      return false;
    }

    inline T read_next()
    {
      return(m_DataMap[m_iCurrentElem++]);
    }

    inline T operator[](size_t i)
    {
      return (m_DataMap[i]);
    }

    inline DWORD get_size()
    {
      struct stat stFile;
        
      fstat(m_fdFile, &stFile); 
      return(stFile.st_size);
    }

    inline bool eof()
    {
      return (m_dwFileSize <= ((m_iCurrentElem + 1) * sizeof(T) + m_dwSizeOfHeader));
    }
    
    inline BYTE* get_header()
    {
      return (m_FileMap);
    }
    
    ~file_map()
    {     
      if (m_bHand)
      {
        lseek(m_fdFile, 0, SEEK_SET);
        write(m_fdFile, m_FileMap, m_dwFileSize);
        //free(m_FileMap);
      }
      else
      {      
        munmap(m_FileMap, m_dwFileSize);
      }
      close(m_fdFile);
    }
  };

  template<>
  class file_map<bit>
  {
  private:
    int m_fdFile;
   
    BYTE* m_DataMap;
    BYTE* m_FileMap;
    
    DWORD m_dwFileSize; 
    
    DWORD m_iCurrentByte;
    DWORD m_nBitsInCurrentByte;   
    BYTE m_CurrentByte;
     
    DWORD m_dwSizeOfHeader;
    bool m_bEof;

    file_map<bit>& operator=(const file_map<bit>&);
    file_map(const file_map<bit>&);

  public:

    class bad_file { };

    file_map(DWORD dwSizeOfHeader = 8): m_fdFile(-1), 
                                        m_dwFileSize(0),
                                        m_iCurrentByte(0), 
                                        m_nBitsInCurrentByte(0),
                                        m_CurrentByte(0),
                                        m_dwSizeOfHeader(dwSizeOfHeader),
                                        m_bEof(false)
    {
      
    }
     
    file_map(char* szFileName, DWORD dwSizeOfHeader = 8, DWORD dwFileSize = 0):

                                  m_fdFile(-1), 
                                  m_dwFileSize(dwFileSize), 
                                  m_iCurrentByte(0), 
                                  m_nBitsInCurrentByte(0),
                                  m_CurrentByte(0),
                                  m_dwSizeOfHeader(dwSizeOfHeader),
                                  m_bEof(false)
    {
      if(!this->create(szFileName, dwFileSize))
        throw new bad_file();
    }

    bool create(char* szFileName, DWORD dwFileSize = 0)
    {
      m_fdFile = open(szFileName, O_RDWR);
      
      ASSERT(m_fdFile != -1);
      if (m_fdFile != -1)
      {
        if (dwFileSize == 0)
        {
          struct stat stFile;
        
          fstat(m_fdFile, &stFile);     
          m_dwFileSize = stFile.st_size;
        }
        else
        {
          m_dwFileSize = dwFileSize;
        }
        
              m_FileMap = (BYTE*)mmap(0, m_dwFileSize, PROT_READ | PROT_WRITE, 
                                      MAP_PRIVATE, m_fdFile, 0);
        m_DataMap = m_FileMap + m_dwSizeOfHeader;
          
        ASSERT(m_FileMap != MAP_FAILED);
        if (m_FileMap != MAP_FAILED)
        {
          m_iCurrentByte = 0;
          m_nBitsInCurrentByte = 0;
          m_CurrentByte = 0;
          return true;
        }               
      }
      return false;
    }

    inline bit read_next()
    {
      if (m_nBitsInCurrentByte == 0) 
      {
        m_CurrentByte = m_DataMap[m_iCurrentByte++];
        m_nBitsInCurrentByte = 8;
      }
    
      bit retval = (MSB(m_CurrentByte) ? 1 : 0);
      m_CurrentByte <<= 1;
      --m_nBitsInCurrentByte;
    
      return (retval);
    }

    inline bit operator[](size_t i)
    {
      BYTE TargetByte = m_DataMap[i / 8];
      bit TargetBit = ( ( TargetByte & (1 << ((i % 8) - 1)) ) == 0 ? 0 : 1);
      return TargetBit;
    }

    inline DWORD get_size()
    {
      struct stat stFile;
      fstat(m_fdFile, &stFile); 
      return(stFile.st_size);
    }

    inline BYTE* get_header()
    {
      return (m_FileMap);
    }

    inline bool eof()
    {
      return (m_dwFileSize == (m_iCurrentByte + 1 + m_dwSizeOfHeader));
    }
    
    ~file_map()
    {
      munmap(m_FileMap, m_dwFileSize);
      close(m_fdFile);
    }
  };


#endif //FILE_MAP_H

