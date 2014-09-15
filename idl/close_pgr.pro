function close_pgr

COMPILE_OPT IDL2

error = call_external('idlpgr.so', 'close_pgr')

return, error
end
