;+
; NAME:
;    DGGhwPointGrey
;
; PURPOSE:
;    Object interface for a PointGrey video camera
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
;    [ G ] PROPERTIES: list of supported PointGrey properties
;    [ GS] BRIGHTNESS
;    [ GS] AUTO_EXPOSURE
;    [ GS] SHARPNESS
;    [ GS] WHITE_BALANCE
;    [ GS] HUE
;    [ GS] SATURATION
;    [ GS] GAMMA
;    [ GS] IRIS
;    [ GS] FOCUS
;    [ GS] ZOOM
;    [ GS] PAN
;    [ GS] TILT
;    [ GS] SHUTTER: Exposure time in milliseconds
;    [ GS] GAIN   : Gain in dB
;    [ GS] TRIGGER_MODE
;    [ GS] TRIGGER_DELAY
;    [ GS] FRAME_RATE
;    [ GS] TEMPERATURE
;
;    [ G ] CAMERAINFO: structure of camera information
;    [ GS] POWER: If set, camera is powered.
;    [ GS] HFLIP: If set, flip image horizontally
;
; METHODS:
;    GetProperty, property = property, ...
;    SetProperty, property = value
;
;    PropertyInfo(property)
;       Returns a structure of information about the specified
;       property
;
;    ControlProperty, property, keywords
;       KEYWORD FLAGS:
;          ON (OFF): If set, turn property ON (or OFF).
;          AUTO (MANUAL): If set, property will be set automatically
;              (manually).
;          ONEPUSH: If set, property will be adjusted automatically
;              and then will return to manual mode.
;
;    ReadRegister(address)
;        Read the value stored in the specified register
;
;    WriteRegister, address, value
;        Write value to the register at the specified address
;
;    Read(): acquire next image from camera and return the data
;
; MODIFICATION HISTORY:
; 07/21/2013 Written by David G. Grier, New York University
; 03/05/2015 DGG Revised for DLM interface.
;
; Copyright (c) 2013-2015 David G. Grier
;-

;;;;;
;
; DGGhwPointGrey::Read()
;
; Return next video frame from camera
;
function DGGhwPointGrey::Read

  COMPILE_OPT IDL2, HIDDEN

  return, idlpgr_RetrieveBuffer(self.context, self.image)
end

;;;;;
;
; DGGhwPointGrey::PropertyInfo()
;
; Get information about specified property
;
function DGGhwPointGrey::PropertyInfo, property

  COMPILE_OPT IDL2, HIDDEN

  if (error = ~self.properties.haskey(property)) then $
     return, error

  return, idlpgr_GetPropertyInfo(self.context, self.properties[property])
end

;;;;;
;
; DGGhwPoingGrey::ControlProperty
;
pro DGGhwPointGrey::ControlProperty, property, $
                                     on = on, $
                                     off = off, $
                                     auto = auto, $
                                     manual = manual, $
                                     onepush = onepush

  COMPILE_OPT IDL2, HIDDEN

  if n_params() ne 1 then $
     return

  if ~self.properties.haskey(property) then $
     return

  propid = self.properties[property]
  prop = idlpgr_GetProperty(self.context, propid)
  info = idlpgr_GetPropertyInfo(self.context, propid)

  if isa(on, /number, /scalar) && info.onoffsupported then $
     prop.onoff = ~keyword_set(on)

  if isa(off, /number, /scalar) && info.onoffsupported then $
     prop.onoff = keyword_set(off)

  if isa(auto, /number, /scalar) && info.autosupported then $
     prop.autoManualMode = ~keyword_set(auto)

  if isa(manual, /number, /scalar) && info.autosupported then $
     prop.autoManualMode = keyword_set(manual)

  if isa(onepush, /number, /scalar) &&  info.onepushsupported then $
     print, 'not implemented yet'

  idlpgr_SetProperty, self.context, prop
end

;;;;;
;
; DGGhwPointGrey::SetProperty
;
pro DGGhwPointGrey::SetProperty, power = power, $
                                 hflip = hflip, $
                                 _ref_extra = propertylist

  COMPILE_OPT IDL2, HIDDEN
  
  if isa(propertylist) then begin
     foreach name, strlowcase(propertylist) do begin
        if self.properties.haskey(name) then begin
           propertyid = self.properties[name]
           info = idlpgr_GetPropertyInfo(self.context,  propertyid)
           if ~info.present then begin
              message, name + ' is not a valid property for this camera. Skipping', /inf
              continue
           endif
           prop = idlpgr_GetProperty(self.context, propertyid)
           if info.absValSupported then begin
              prop.abscontrol = 1L
              value = float(scope_varfetch(name, /ref_extra))
              value >= info.absmin
              value <= info.absmax
              prop.absvalue = value
           endif else begin
              value = long(scope_varfectch(name, /ref_extra))
              value >= info.min
              value <= info.max
              prop.valueA = value
           endelse
           idlpgr_SetProperty, self.context, prop
        endif
     endforeach
  endif

  if isa(hflip, /number, /scalar) then begin
     value = '80000000'XUL + (hflip ne 0)
     self.writeregister, '1054'XUL, value
  endif

  if isa(power, /number, /scalar) then begin
     power = self.readregister('610'XUL)
     power or= ulong(keyword_set(power))
     print, power
     self.writeregister, '610'XUL, power
  endif
end

;;;;;
;
; DGGhwPointGrey::GetProperty
;
pro DGGhwPointGrey::GetProperty, power      = power, $
                                 properties = properties, $
                                 camerainfo = camerainfo, $
                                 hflip      = hflip,      $
                                 _ref_extra = propertylist

  COMPILE_OPT IDL2, HIDDEN

  if isa(propertylist) then begin
     foreach name, strlowcase(propertylist) do begin
        if self.properties.haskey(name) then begin
           propertyid = self.properties[name]
           info = idlpgr_GetPropertyInfo(self.context, propertyid)
           if info.present then begin
              prop = idlpgr_GetProperty(self.context, propertyid)
              (scope_varfetch(name, /ref_extra)) = (info.absValSupported) ? $
                                                   prop.absvalue : prop.valueA
           endif else begin
              message, name + ' is not a valid property for this camera.', /inf
              (scope_varfetch(name, /ref_extra)) = 0
           endelse
        endif
     endforeach
  endif

  if arg_present(properties) then $
     properties = self.properties.keys()

  if arg_present(camerainfo) then $
     camerainfo = idlpgr_GetCameraInfo(self.context)

  if arg_present(hflip) then $
     hflip = (self.readregister('1054'XUL) and 1)

  if arg_present(power) then $
     power = self.readregister('610'XUL)
end

;;;;;
;
; DGGhwPointGrey::WriteRegister
;
pro DGGhwPointGrey::WriteRegister, address, value

  COMPILE_OPT IDL2, HIDDEN

  idlpgr_WriteRegister, self.context, address, value
end

;;;;;
;
; DGGhwPointGrey::ReadRegister()
;
function DGGhwPointGrey::ReadRegister, address

  COMPILE_OPT IDL2, HIDDEN

  return, idlpgr_ReadRegister(self.context, address)
end

;;;;;
;
; DGGhwPointGrey::Init()
;
function DGGhwPointGrey::Init, camera = _camera

  COMPILE_OPT IDL2, HIDDEN

  catch, error
  if (error ne 0L) then begin
     catch, /cancel
     return, 0B
  endif

  camera = isa(_camera, /number, /scalar) ? long(_camera) : 0L

  self.context = idlpgr_CreateContext()
  camera = idlpgr_GetCameraFromIndex(self.context, camera)
  idlpgr_Connect, self.context, camera
  idlpgr_StartCapture, self.context
  self.image =  idlpgr_CreateImage(self.context)

  properties = ['brightness',    $
                'auto_exposure', $
                'sharpness',     $
                'white_balance', $
                'hue',           $
                'saturation',    $
                'gamma',         $
                'iris',          $
                'focus',         $
                'zoom',          $
                'pan',           $
                'tilt',          $
                'shutter',       $
                'gain',          $
                'trigger_mode',  $
                'trigger_delay', $
                'frame_rate',    $
                'temperature']
  indexes = indgen(n_elements(properties))
  self.properties = orderedhash(properties, indexes)

  a = idlpgr_RetrieveBuffer(self.context, self.image)

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

  idlpgr_StopCapture, self.context
  idlpgr_DestroyContext, self.context
  idlpgr_DestroyImage, self.image
end

;;;;;
;
; DGGhwPointGrey
;
; Object for interacting with a PointGrey camera
;
pro DGGhwPointGrey__define

  COMPILE_OPT IDL2

  struct = {DGGhwPointGrey, $
            inherits IDL_Object, $
            context: 0ULL,  $
            image: bytarr(48),  $
            properties: obj_new() $
           }
end
