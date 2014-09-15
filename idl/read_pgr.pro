function read_pgr, camera

COMPILE_OPT IDL2

a = bytarr(camera.nx, camera.ny, /nozero)
error = call_external('idlpgr.so', 'read_pgr', a)

return, (error) ? error : a
end
