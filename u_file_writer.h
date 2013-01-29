

#ifndef FILE_WRITER_H__
#define FILE_WRITER_H__
  
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


  template<class T>
  class file_writer
  {
    private:
      int m_fdFile;
      T* m_tBuffer;

      DWORD m_dwCurrentElem;
      DWORD m_dwElemsInBuffer;
   
    public:
      file_writer() : m_fdFile(-1)
      {
        m_tBuffer = (T*)malloc(4 * 1024);
        if(m_tBuffer)
        {
          m_dwCurrentElem = 0;
          m_dwElemsInBuffer = (4 * 1024) / sizeof(T);
        }
      }

      inline bool is_good()
      {
        return (m_fdFile != -1);
      }

      inline bool create(char* szFilename)
      {
        m_fdFile = creat(szFilename, 0666);         
        return(is_good());
      }

      inline bool send(T t)
      {
        m_tBuffer[m_dwCurrentElem] = t;
        if (++m_dwCurrentElem == m_dwElemsInBuffer)
        {
          DWORD nBytesWritten = write(m_fdFile, m_tBuffer, 
                                    m_dwElemsInBuffer * sizeof(T));
          
          m_dwCurrentElem = 0;
          ASSERT(nBytesWritten == (m_dwElemsInBuffer * sizeof(T)));
          if (nBytesWritten != (m_dwElemsInBuffer * sizeof(T))) 
            return false;
        }
        return true;
      }

      inline void write_header(BYTE* Buffer, int cbSize)
      {
        /*DWORD dwBuffer[2];
        dwBuffer[0] = g_dwMagic;
        dwBuffer[1] = dwExpSize;*/

        int nBytesWritten = write(m_fdFile, Buffer, cbSize);

        ASSERT(nBytesWritten == cbSize);
      }

      ~file_writer()
      {
        DWORD nBytesWritten = write(m_fdFile, m_tBuffer, m_dwCurrentElem * sizeof(T));
        
        ASSERT(nBytesWritten == (m_dwCurrentElem * sizeof(T)));    
        close(m_fdFile);
        free(m_tBuffer);
      }
  };

  template<>
  class file_writer<bit>
  {
    private:
      int m_fdFile;
      BYTE* m_Buffer;
      BYTE m_bitBuffer;

      DWORD m_dwCurrentByte;

      DWORD m_dwBytesInBuffer;
      DWORD m_dwBitsInByte;

    public:  
      file_writer() : m_fdFile(-1)
      {
        m_Buffer = (BYTE*)malloc(4 * 1024);
        if(m_Buffer)
        {
          m_dwCurrentByte = 0;
          m_dwBytesInBuffer = 4 * 1024;
          m_dwBitsInByte = 8;
          m_bitBuffer = 0;
        }
      }

      inline bool is_good()
      {
        return (m_fdFile != -1);
      }

      inline bool create(char* szFilename)
      {
        m_fdFile = creat(szFilename, 0666);         
        return(is_good());
      }

      inline bool send(bit b)
      {
        if (m_dwBitsInByte == 0)
        {
          m_Buffer[m_dwCurrentByte] = m_bitBuffer; 

          if (++m_dwCurrentByte == m_dwBytesInBuffer)
          {      
            DWORD nBytesWritten = write(m_fdFile, m_Buffer, m_dwBytesInBuffer);
          
            m_dwCurrentByte = 0;
            
            ASSERT(nBytesWritten == m_dwBytesInBuffer);
            if (nBytesWritten != m_dwBytesInBuffer)
              return false;
          }

          m_dwBitsInByte = 8;
          m_bitBuffer = 0;
        }

        --m_dwBitsInByte;
        m_bitBuffer <<= 1;

        if (b) m_bitBuffer += 0x1;  
        
        return true;
      }

      inline void write_header(BYTE* Buffer, int cbSize)
      {
        int nBytesWritten = write(m_fdFile, Buffer, cbSize);
        ASSERT(nBytesWritten == cbSize);
      }

      ~file_writer()
      {        
        DWORD nBytesWritten;
        
        nBytesWritten = write(m_fdFile, m_Buffer, m_dwCurrentByte);       
        ASSERT(nBytesWritten == m_dwCurrentByte);

        while(m_dwBitsInByte)
        {
          m_bitBuffer <<= 1;
          --m_dwBitsInByte;
        }

        nBytesWritten = write(m_fdFile, &m_bitBuffer, sizeof(BYTE));
        ASSERT(nBytesWritten == sizeof(BYTE));
    
        close(m_fdFile);
        free(m_Buffer);
      }
  }; 

#endif //FILE_WRITER_H

