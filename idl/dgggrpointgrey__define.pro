;+
; NAME:
;    DGGgrPointGrey
;
; PURPOSE:
;    Object interface for a PointGrey video camera
;    suitable for inclusion in an object graphics hierarchy.
;
; CATEGORY:
;    Hardware automation, Video processing
;
; SUBCLASSES:
;    DGGhwPointGrey
;    IDL_Object
;
; CALLING SEQUENCE:
;    a = DGGgrPointGrey
;
; PROPERTIES:
;    Same as underlying DGGhwPointGrey object.
;
; METHODS:
;    Same as underlying DGGhwPointGrey object,
;    except with IDL_Object extensions
;
; MODIFICATION HISTORY:
; 03/06/2015 Written by David G. Grier, New York University
;
; Copyright (c) 2015 David G. Grier
;-

;;;;;
;
; DGGgrPointGrey__define
;
pro DGGgrPointGrey__define

  COMPILE_OPT IDL2, HIDDEN

  struct = {DGGgrPointGrey, $
            inherits DGGhwPointGrey, $
            inherits IDL_Object $
           }
end
