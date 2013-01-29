
#include "common.h"
#include "arithmetic_encoder.h"

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
#endif

#include <iostream>

using namespace std;

void print_usage()
{
  cout << "\nUsage: encode -enc=x input output\n\n"
       << "       -enc=x x: modello da utilizzare\n"
       << "              1: modello ppm\n"
       << "              2: modello jpeg-ls per bitmap a 24bpp\n"
       << "              3: modello jpeg-ls per bitmap a 8bpp\n"
       << "              4: modello adattivo per BYTE (generale)\n"
       << "              5: modello differenziale per BYTE\n"
       << "\nCopyright (C) 2001 Lorenzo Dematte'. Tutti i diritti riservati\n";
    
}

void encode_ppm(char* szInputName, char* szOutputName)
{
  file_map<char> map_input(szInputName);
  file_writer<bit> file_comp_out;
  file_comp_out.create(szOutputName);

  arithmetic_encoder enc(&file_comp_out);
  ppm_model<5> mdl(&enc);

  DWORD adwHeader[2];
  
  adwHeader[0] = g_dwMagic;
  adwHeader[1] = map_input.get_size();

  file_comp_out.write_header((BYTE*)adwHeader, sizeof(adwHeader));
  
  for(;;)
  {
    char c = map_input.read_next();
    if (map_input.eof()) 
    {
      mdl.encode_eof();
      break;
    }
    mdl.encode_symbol(c);
  }  
}

void encode_ls24(char* szInputName, char* szOutputName)
{ 
  bitmap_true<rgb_color24> bmpFileInput(szInputName);

  file_writer<bit> file_comp_out;
  file_comp_out.create(szOutputName);

  file_comp_out.write_header((BYTE*)(bmpFileInput.get_header()),
                                     bmpFileInput.get_header()->get_header_size());

  arithmetic_encoder enc(&file_comp_out);
  jpeg_ls_24_model mdl(&enc, &bmpFileInput);

  for(DWORD i = 0; i < bmpFileInput.height(); ++i)
    for(DWORD j = 0; j < bmpFileInput.width(); ++j)
    {
      rgb_color24 rgbPixel = bmpFileInput.get_pixel(i, j);
      
      mdl.predict5(i, j);
      mdl.encode_symbol(rgbPixel);
    }

  mdl.encode_eof();
}

void encode_ls8(char* szInputName, char* szOutputName)
{
  bitmap8bpp bmpFileInput(szInputName);

  file_writer<bit> file_comp_out;
  file_comp_out.create(szOutputName);

  file_comp_out.write_header((BYTE*)(bmpFileInput.get_header()),
                                     bmpFileInput.get_header()->get_header_size());

  arithmetic_encoder enc(&file_comp_out);
  jpeg_ls_model mdl(&enc, &bmpFileInput);

  for(DWORD i = 0; i < bmpFileInput.height(); ++i)
    for(DWORD j = 0; j < bmpFileInput.width(); ++j)
    {
      BYTE bPixel = bmpFileInput.get_greyed_pixel(i, j);
      
      mdl.predict5(i, j);
      mdl.encode_symbol(bPixel);
    }

  mdl.encode_eof();
}

void encode_BYTE(char* szInputName, char* szOutputName)
{
  file_map<BYTE> map_input(szInputName);
  file_writer<bit> file_comp_out;
  file_comp_out.create(szOutputName);

  arithmetic_encoder enc(&file_comp_out);
  adaptive_model<BYTE> mdl(&enc);

  DWORD adwHeader[2];
  
  adwHeader[0] = g_dwMagic;
  adwHeader[1] = map_input.get_size();

  file_comp_out.write_header((BYTE*)adwHeader, sizeof(adwHeader));
  
  for(;;)
  {
    BYTE b = map_input.read_next();
    if (map_input.eof()) 
    {
      mdl.encode_eof();
      break;
    }
    mdl.encode_symbol(b);
  }
}

void encode_diff_BYTE(char* szInputName, char* szOutputName)
{
  file_map<BYTE> map_input("lenna.bmp");
  file_writer<bit> file_comp_out;
  file_comp_out.create("lenna1.comp");

  arithmetic_encoder enc(&file_comp_out);
  difference_adaptive_model<BYTE> mdl(&enc);

  DWORD adwHeader[2];
  
  adwHeader[0] = g_dwMagic;
  adwHeader[1] = map_input.get_size();

  file_comp_out.write_header((BYTE*)adwHeader, sizeof(adwHeader));
  
  for(;;)
  {
    BYTE b = map_input.read_next();
    if (map_input.eof()) 
    {
      mdl.encode_eof();
      break;
    }
    mdl.encode_symbol(b);
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
      encode_ppm(szInputName, szOutputName);
      break;
    
    case 2:
      encode_ls24(szInputName, szOutputName);
      break;
    
    case 3:
      encode_ls8(szInputName, szOutputName);
      break;
    
    case 4:
      encode_BYTE(szInputName, szOutputName);
      break;
  
    case 5:
      encode_diff_BYTE(szInputName, szOutputName);
      break;

    default:
      cout << "Modello " << dwEncoderType << " non riconosciuto!" << endl;
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

