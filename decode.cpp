
#include "common.h"
#include "arithmetic_decoder.h"

#include "difference_adaptive_model.h"
#include "jpeg_ls_model.h"
#include "bitmap.h"
#include "adaptive_model.h"
#include "ppm_model.h"

#ifdef WINDOWS
#include "file_map.h"
#include "file_writer.h"
#else
#include "u_file_map.h"
#include "u_file_writer.h"
#endif //WINDOWS

#include <iostream>

using namespace std;

void print_usage()
{
  cout << "\nUsage: decode -enc=x input output\n\n"
       << "       -enc=x x: modello da utilizzare\n"
       << "              1: modello ppm\n"
       << "              2: modello jpeg-ls per bitmap a 24bpp\n"
       << "              3: modello jpeg-ls per bitmap a 8bpp\n"
       << "              4: modello adattivo per BYTE (generale)\n"
       << "              5: modello differenziale per BYTE\n"
       << "\nCopyright (C) 2001 Lorenzo Dematte'. Tutti i diritti riservati\n";
    
}

void decode_ls24(char* szInputName, char* szOutputName)
{  
  DWORD dwFileSize, dwHeaderSize;
                                        
  {
    bitmap_header* bhCompInput;

    file_map<bit> mapCompressedInput(szInputName, sizeof(bitmap_header));
    bhCompInput = (bitmap_header*)(mapCompressedInput.get_header());

    dwHeaderSize = bhCompInput->get_header_size();
    dwFileSize = bhCompInput->get_file_size();

    file_writer<bit> bmpWriter;
    bmpWriter.create(szOutputName);
    bmpWriter.write_header((BYTE*)bhCompInput, bhCompInput->get_header_size());
  }
  
  file_map<bit> mapCompressedInput(szInputName, dwHeaderSize);  
  bitmap_true<rgb_color24> bmpFileOut(szOutputName, dwFileSize);
  
  arithmetic_decoder decoder(&mapCompressedInput);
  jpeg_ls_24_model model(&decoder, &bmpFileOut);

  for (DWORD i = 0; i < bmpFileOut.height(); ++i)
    for (DWORD j = 0; j < bmpFileOut.width(); ++j)
    {      
      model.predict5(i, j);
      rgb_color24 rgb = model.decode_symbol();      
      bmpFileOut.set_pixel(i, j, rgb); 
    }
}

void decode_ppm(char* szInputName, char* szOutputName)
{
  file_map<bit> map_comp_input(szInputName);
  file_writer<char> file_out;
  file_out.create(szOutputName);
  
  arithmetic_decoder dec(&map_comp_input);
  ppm_model<5> mdl(&dec);

  DWORD* dwBuffer = (DWORD*)map_comp_input.get_header();
  
  DWORD dwExpSize = dwBuffer[1];

  if (dwBuffer[0] == g_dwMagic)
  {
    for(DWORD i = 0; i < (dwExpSize / sizeof(BYTE)); ++i)
    {
      char c = mdl.decode_symbol();
      file_out.send(c);
    }
  }
}


void decode_ls8(char* szInputName, char* szOutputName)
{
  DWORD dwFileSize, dwHeaderSize;
                                        
  {
    bitmap_header* bhCompInput;

    file_map<bit> mapCompressedInput(szInputName, sizeof(bitmap_header));
    bhCompInput = (bitmap_header*)(mapCompressedInput.get_header());

    dwHeaderSize = bhCompInput->get_header_size();
    dwFileSize = bhCompInput->get_file_size();

    file_writer<bit> bmpWriter;
    bmpWriter.create(szOutputName);
    bmpWriter.write_header((BYTE*)bhCompInput, bhCompInput->get_header_size());
  }
  
  file_map<bit> mapCompressedInput(szInputName, dwHeaderSize);  
  bitmap8bpp bmpFileOut(szOutputName, dwFileSize);
  
  arithmetic_decoder decoder(&mapCompressedInput);
  jpeg_ls_model model(&decoder, &bmpFileOut);

  for (DWORD i = 0; i < bmpFileOut.height(); ++i)
    for (DWORD j = 0; j < bmpFileOut.width(); ++j)
    {
      model.predict5(i, j);
      BYTE b = model.decode_symbol();
      bmpFileOut.set_greyed_pixel(i, j, b); 
    } 
}

void decode_BYTE(char* szInputName, char* szOutputName)
{
  file_map<bit> map_comp_input(szInputName);
  file_writer<BYTE> file_out;
  file_out.create(szOutputName);
  
  arithmetic_decoder dec(&map_comp_input);
  adaptive_model<BYTE> mdl(&dec);

  DWORD* dwBuffer = (DWORD*)map_comp_input.get_header();
  
  DWORD dwExpSize = dwBuffer[1];

  if (dwBuffer[0] == g_dwMagic)
  {
    for(DWORD i = 0; i < (dwExpSize / sizeof(BYTE)); ++i)
    {
      BYTE b = mdl.decode_symbol();
      file_out.send(b);
    }
  }
}

void decode_diff_BYTE(char* szInputName, char* szOutputName)
{
  file_map<bit> map_comp_input(szInputName);
  file_writer<BYTE> file_out;
  file_out.create(szOutputName);
  
  arithmetic_decoder dec(&map_comp_input);
  difference_adaptive_model<BYTE> mdl(&dec);

  DWORD* dwBuffer = (DWORD*)map_comp_input.get_header();
  
  DWORD dwExpSize = dwBuffer[1];

  if (dwBuffer[0] == g_dwMagic)
  {
    for(DWORD i = 0; i < (dwExpSize / sizeof(BYTE)); ++i)
    {
      BYTE b = mdl.decode_symbol();
      file_out.send(b);
    }
  }
}

int main (int argc, char* argv[])
{
  char* szInputName = NULL;
  char* szOutputName = NULL;
  DWORD dwEncoderType = 0;
  
  //parse the command line
  
  if (argc != 4)
  {
    print_usage();
    return -1;
  }
  
  if(strncmp(argv[1], "-enc=", 5) == 0)
  {
    dwEncoderType = atoi(&(argv[1][5]));
  }
  else 
  {
    print_usage();
    return -1;
  }
  
  szInputName = argv[2];
  szOutputName = argv[3];
  
  if ((!szInputName) || (!szOutputName))
  {
    print_usage();
    return -1;
  }
 
  try
  {
    switch(dwEncoderType)
    {
    case 1:
      decode_ppm(szInputName, szOutputName);
      break;
    
    case 2:
      decode_ls24(szInputName, szOutputName);
      break;
    
    case 3:
      decode_ls8(szInputName, szOutputName);
      break;
    
    case 4:
      decode_BYTE(szInputName, szOutputName);
      break;
  
    case 5:
      decode_diff_BYTE(szInputName, szOutputName);
      break;
    }
  }
  catch(bad_file*)
  {
    cout << "\nERRORE:\n"
         << "Impossibile aprire o creare i files necessari.\n"
         << "Controllare che i files esistano, non siano bloccati "
         << "da altri processi e che si abbiano i permessi necessari.\n\n";
  }
  catch(wrong_bpp*)
  {
    cout << "\nERRORE:\n"
         << "Il file bitmap specificato ha una profondita' di colore differente "
         << "da quella indicata: provare a specificare un formato diverso\n\n";
  }
  catch(...)
  {
    cout << "\nERRORE:\n"
         << "Allocazione memoria fallita!\n\n";      
  }
 
  return 0;
}

