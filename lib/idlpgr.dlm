MODULE idlpgr
DESCRIPTION IDL interface to the PointGrey FlyCap2 API
VERSION 0.1
SOURCE David G. Grier, New York University
BUILD_DATE MAR 03 2015
FUNCTION IDLPGR_CREATECONTEXT 0 0
PROCEDURE IDLPGR_DESTROYCONTEXT 1 1
FUNCTION IDLPGR_GETNUMOFCAMERAS 1 1
FUNCTION IDLPGR_GETCAMERAFROMINDEX 1 2
PROCEDURE IDLPGR_CONNECT 2 2
PROCEDURE IDLPGR_STARTCAPTURE 1 1
PROCEDURE IDLPGR_STOPCAPTURE 1 1
FUNCTION IDLPGR_CREATEIMAGE 1 1
PROCEDURE IDLPGR_DESTROYIMAGE 1 1
FUNCTION IDLPGR_RETRIEVEBUFFER 2 2
FUNCTION IDLPGR_READREGISTER 2 2
PROCEDURE IDLPGR_WRITEREGISTER 3 3
FUNCTION IDLPGR_GETPROPERTYINFO 2 2
FUNCTION IDLPGR_READPROPERTY 2 2
PROCEDURE IDLPGR_SETPROPERTY 3, 3