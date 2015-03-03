function read_pgr, pgr

COMPILE_OPT IDL2

if ~isa(pgr, 'IDLPGR') then $
   return, 0

return, idlpgr_RetrieveBuffer(pgr.context, pgr.image)
end
