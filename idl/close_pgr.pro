pro close_pgr, pgr

COMPILE_OPT IDL2

if isa(pgr, 'IDLPGR') then begin
   idlpgr_StopCapture, pgr.context
   idlpgr_DestroyContext, pgr.context
   idlpgr_DestroyImage, pgr.image
endif
end
