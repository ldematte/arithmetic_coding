
all: encode decode

encode: encode.cpp arithmetic_encoder.h  arithmetic_decoder.h index.h \
 u_file_map.h types.h common.h model.h adaptive_model.h u_file_writer.h \
 bitmap.h bmpDEF.h rgb_color.h jpeg_ls_model.h difference_adaptive_model.h \
 ppm_model.h
	g++ -O4 -o encode encode.cpp

decode: decode.cpp arithmetic_decoder.h arithmetic_encoder.h index.h \
 u_file_writer.h u_file_map.h types.h common.h model.h adaptive_model.h \
 bitmap.h bmpDEF.h rgb_color.h jpeg_ls_model.h difference_adaptive_model.h \
 ppm_model.h
	g++ -O4 -o decode decode.cpp

clean:
	rm -f .ctex *.log *.dvi *.aux *.o *.cpp *.h *.ctex encode decode

