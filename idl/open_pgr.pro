function open_pgr

COMPILE_OPT IDL2

context = idlpgr_CreateContext()
camera = idlpgr_GetCameraFromIndex(context, 0)
idlpgr_Connect, context, camera
idlpgr_StartCapture, context
image = idlpgr_CreateImage(context)

pgr = {IDLPGR, $
       context: context, $
       image: image}

return, pgr
end
