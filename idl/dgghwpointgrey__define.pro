;+
; NAME:
;    DGGhwPointGrey
;
; PURPOSE:
;    Object interface to a Point Grey video camera
;
; CATEGORY:
;    Hardware automation, Video processing
;
; CALLING SEQUENCE:
;    a = DGGhwPointGrey()
;
; SUBCLASSES:
;    IDL_OBJECT
;
; PROPERTIES:
;    dim   [ G ] dimensions of the image returned by the camera
;                dim[0]: width [pixels]
;                dim[2]: height [pixels]
;
;    data  [ G ] [nx, ny] array of byte-valued data
;    
;    gray  [ G ] Flag: grayscale if set
;
; METHODS:
;    GetProperty
;    SetProperty
;
;    Read(): acquire next image from camera and return the data
;    Read  : acquire next image from camera
;
; MODIFICATION HISTORY:
; 07/21/13 Written by David G. Grier, New York University
;
; Copyright (c) 2013 David G. Grier
;-

;;;;;
;
; DGGhwPointGrey::Read
;
; Acquire next video frame from camera
;
pro DGGhwPointGrey::Read

COMPILE_OPT IDL2, HIDDEN

error = call_external(self.dlm, 'read_pgr', *self.data)

end

;;;;;
;
; DGGhwPointGrey::Read()
;
; Return next video frame from camera
;
function DGGhwPointGrey::Read

COMPILE_OPT IDL2, HIDDEN

self.read
return, *self.data
end

;;;;;
;
; DGGhwPointGrey::GetProperty
;
pro DGGhwPointGrey::GetProperty, data = data, $
                                 dim = dim, $
                                 gray = gray
COMPILE_OPT IDL2, HIDDEN

if arg_present(data) then data = *self.data
if arg_present(dim) then dim = self.dim
if arg_present(gray) then gray = self.gray

end

;;;;;
;
; DGGhwPointGrey::Init()
;
; Initialize FlyCapture2_C library, connect to camera
;
function DGGhwPointGrey::Init

COMPILE_OPT IDL2, HIDDEN

self.dlm = '/usr/local/IDL/idlpgr/idlpgr.so'
nx = 0
ny = 0
error = call_external(self.dlm, 'open_pgr', nx, ny) 
if error then $
   return, 0B

self.dim = [nx, ny]
self.gray = 1B

data = bytarr(nx, ny)
self.data = ptr_new(data, /no_copy)

return, 1B
end

;;;;;
;
; DGGhwPointGrey::Cleanup
;
; Free resources from FlyCapture2_C library
;
pro DGGhwPointGrey::Cleanup

COMPILE_OPT IDL2, HIDDEN

if ptr_valid(self.data) then $
   ptr_free, self.data

error = call_external(self.dlm, 'close_pgr')

end

;;;;;
;
; DGGhwPointGrey
;
; Object for acquiring images from a Point Grey camera
;
pro DGGhwPointGrey__define

COMPILE_OPT IDL2

struct = {DGGhwPointGrey, $
	  INHERITS IDL_OBJECT, $
          dlm: '', $
          dim: [0, 0], $
          data: ptr_new(), $
          gray: 0B $
         }

end
