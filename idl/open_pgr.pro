function open_pgr

COMPILE_OPT IDL2

nx = 0
ny = 0
error = call_external('idlpgr.so', 'open_pgr', nx, ny)
if ~error then begin
  s = {pgr, $
       nx: nx, $ ; width of image
       ny: ny  $ ; height of image
      }
  return, s
endif

return, error
end
