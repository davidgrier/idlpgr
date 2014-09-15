;;;;;
;
; testpgr
;
; Simple routine to test the IDL interface for
; Point Grey cameras
;
; MODIFICATION HISTORY:
; 07/22/2013 Written by David G. Grier, New York University
;
; Copyright (c) 2013 David G. Grier
;

camera = DGGhwPointGrey()
print, "Acquiring 100 frames ..."
tic
for i = 0, 99 do $
  a = camera.read()
toc
print, "... done"
exit
