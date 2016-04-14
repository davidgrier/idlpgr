;;;;;
;
; compile_idlvideo.pro
;
; IDL batch file to compile the shared library that provides
; an IDL interface for Point Grey cameras.
;
; Modification History:
; 07/19/2013 Written by David G. Grier, New York University
; 04/14/2016 DGG Include local copies of headers.
;
; Copyright (c) 2013-2016 David G. Grier
;
project_directory = './'
compile_directory = './build'
infiles = 'idlpgr'
outfile = 'idlpgr'

extra_cflags = '-I"../../flycapture2/include"'
extra_lflags = '-L"../../flycapture2/lib" -lflycapture-c -lflycapture'

;;;;;
;
; Build the library
make_dll, infiles, outfile, 'IDL_Load', $
          extra_cflags = extra_cflags, $
          extra_lflags = extra_lflags, $
	  input_directory = project_directory, $
          output_directory = project_directory, $
          compile_directory = compile_directory, $
          /platform_extension

exit
