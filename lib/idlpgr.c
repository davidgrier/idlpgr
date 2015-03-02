//
// idlpgr.c
//
// IDL-callable library implementing frame capturing
// from Point Grey cameras.  Based on the flycapture2 library.
//
// Reference:
// http://www.ptgrey.com/support/downloads/documents/flycapture2/
//
// Modification History:
// 07/19/2013 Written by David G. Grier, New York University
// 01/24/2014 DGG Implemented read_register.
// 01/26/2014 DGG Implemented write_register, read_property & write_property
// 03/02/2015 DGG Better integration of FlyCap2 API with IDL.
//
// Copyright (c) 2013-2015 David G. Grier
//
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// IDL support
#include "idl_export.h"

// Point Grey support
#include "C/FlyCapture2_C.h"

fc2Context context;
fc2PGRGuid guid;
fc2Image   image;

// Error messages
static IDL_MSG_DEF msg_arr[] =
  {
#define M_IDLPGR_ERROR      0
    { "M_IDLPGR_ERROR", "%NError: %s" },
#define M_IDLPGR_ERRORCODE -1
    { "M_IDLPGR_ERRORCODE", "%NError: %s Code: %0X" },
  };

static IDL_MSG_BLOCK msgs;

//
// idlpgr_CreateContext
//
IDL_VPTR idlpgr_CreateContext(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;

  error = fc2CreateContext(&context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP, 
			 "Could not create context.", error);

  return IDL_GettmpULong64((IDL_ULONG64) context);
}

//
// idlpgr_DestroyContext
//
void idlpgr_DestroyContext(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;

  IDL_ENSURE_SIMPLE(argv[0]);
  IDL_ENSURE_SCALAR(argv[0]);
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  error = fc2DestroyContext(context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not destroy specified context.", error);
  IDL_StoreScalarZero(argv[0], IDL_TYP_LONG);
}

//
// idlpgr_GetNumOfCameras
//
IDL_VPTR idlpgr_GetNumOfCameras(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  unsigned int ncameras = 0;

  IDL_ENSURE_SIMPLE(argv[0]);
  IDL_ENSURE_SCALAR(argv[0]);
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  error = fc2GetNumOfCameras(context, &ncameras);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not count cameras.", error);
  
  return IDL_GettmpLong(ncameras);
}

//
// idlpgr_GetCameraFromIndex
//
IDL_VPTR idlpgr_GetCameraFromIndex(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  int camera;
  fc2PGRGuid guid;
  IDL_VPTR idl_guid;
  IDL_ULONG *pd;
  IDL_MEMINT dim = 4;
  int i;

  IDL_ENSURE_SIMPLE(argv[0]);
  IDL_ENSURE_SCALAR(argv[0]);
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  camera = (argc == 2) ? (int) IDL_LongScalar(argv[1]) : 0;

  error = fc2GetCameraFromIndex(context, camera, &guid);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not acquire specified camera.", error);

  // transfer camera guid to IDL as a vector of ULONG values
  pd = (IDL_ULONG *) IDL_MakeTempVector(IDL_TYP_ULONG, 
					dim, IDL_ARR_INI_NOP, &idl_guid);
  for (i = 0; i < 4; i++)
    pd[i] = guid.value[i];
  
  return idl_guid;
}

//
// idlpgr_Connect
//
void idlpgr_Connect(int argc, IDL_VPTR argv[])
 {
  fc2Error error;
  fc2Context context;
  fc2PGRGuid guid;
  IDL_MEMINT n;
  IDL_ULONG *pd;
  int i;
  
  IDL_ENSURE_SIMPLE(argv[0]);
  IDL_ENSURE_SCALAR(argv[0]);
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  IDL_EXCLUDE_EXPR(argv[1]);
  IDL_ENSURE_ARRAY(argv[1]);
  if (argv[1]->value.arr->n_elts != 4)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERROR, IDL_MSG_LONGJMP,
			 "Provided variable is not a camera GUID.");
  pd = (IDL_ULONG *) argv[1]->value.arr->data;
  for (i = 0; i < 4; i++)
    guid.value[i] = pd[i];
  error = fc2Connect(context, &guid);
  if (error) 
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not connect camera to context.", error);
}

//
// OPEN_PGR
//
// Initiates communication with Point Grey camera
// and returns parameters describing image geometry.
//
// argv[0]: OUT width
// argv[1]: OUT height
//
IDL_INT IDL_CDECL open_pgr(int argc, char *argv[])
{
  fc2Error error;
  unsigned int ncameras = 0;

  error = fc2CreateContext(&context);
  if (error) {
    fprintf(stderr, "could not create context: %d\n", error);
  }
  error = fc2GetNumOfCameras(context, &ncameras);
  if (ncameras != 1) {
	fprintf(stderr, "%d cameras found\n", ncameras);
	return (IDL_INT) -2;
  }	
  error = fc2GetCameraFromIndex(context, 0, &guid);
  fprintf(stderr, "get camera: %d\n", error);
  error = fc2Connect(context, &guid);
  fprintf(stderr, "connect: %d\n", error);
  error = fc2StartCapture(context);
  fprintf(stderr, "start capture: %d\n", error);
  error = fc2CreateImage(&image);
  fprintf(stderr, "create image: %d\n", error);
  error = fc2RetrieveBuffer(context, &image);
  fprintf(stderr, "retrieve buffer: %d\n", error);

  // return information about video
  *(IDL_INT *) argv[0] = (IDL_INT) image.cols;
  *(IDL_INT *) argv[1] = (IDL_INT) image.rows;

  return (IDL_INT) error;
}

IDL_INT IDL_CDECL close_pgr(void)
{
  fc2Error error;

  error = fc2DestroyImage(&image);
  error = fc2StopCapture(context);
  error = fc2DestroyContext(context);
  return (IDL_INT) error;
}

//
// READ_PGR
//
// Acquire next video frame from camera
//
// argv[0]: OUT image data
//
IDL_INT IDL_CDECL read_pgr(int argc, char *argv[])
{
  fc2Error error;
  unsigned char *data;

  data = argv[0];
  error = fc2RetrieveBuffer(context, &image);
  memcpy(data, image.pData, (size_t) image.dataSize);

  return (IDL_INT) error;
}

//
// READ_REGISTER
//
// Read contents of specified register
//
// argv[0]: IN camera register index
// argv[1]: OUT value in register
//
IDL_INT IDL_CDECL read_register(int argc, char *argv[])
{
  fc2Error error;
  unsigned int address, value;

  address = *(unsigned int *) argv[0];
  // fprintf(stderr, "address %x, size %d\n", address, sizeof(address));
  error = fc2ReadRegister(context, address, &value);
  // fprintf(stderr, "value %x\n", value);
  *(IDL_ULONG *) argv[1] = (IDL_ULONG) value;

  return (IDL_INT) error;
}

//
// WRITE_REGISTER
//
// Write unsigned integer to specified register
//
// argv[0]: IN camera register index
// argv[1]: IN value to be written to register
//
IDL_INT IDL_CDECL write_register(int argc, char *argv[])
{
  fc2Error error;
  unsigned int address, value;

  address = *(unsigned int *) argv[0];
  value = *(unsigned int *) argv[1];
  error = fc2WriteRegister(context, address, value);

  return (IDL_INT) error;
}

//
// PROPERTY_INFO
//
// Get information about property
//
// Reference: FlyCapture2Defs_C.h
//
// argv[0]: IN property type
// argv[1]: OUT present
// argv[2]: OUT autoSupported
// argv[3]: OUT manualSupported
// argv[4]: OUT onOffSupported
// argv[5]: OUT absValSupported
// argv[6]: OUT readOutSupported
// argv[7]: OUT min
// argv[8]: OUT max
// argv[9]: OUT absMin
// argv[10]: OUT absMax
// argv[11]: OUT pUnits -- NOT IMPLEMENTED
// argv[12]: OUT pUnitsAbbr -- NOT IMPLEMENTED
//
IDL_INT IDL_CDECL property_info(int argc, char *argv[])
{
  fc2Error error;
  fc2PropertyInfo info;

  info.type = *(fc2PropertyType *) argv[0];
  error = fc2GetPropertyInfo(context, &info);

  *(IDL_INT *) argv[1] = (IDL_INT) info.present;
  *(IDL_INT *) argv[2] = (IDL_INT) info.autoSupported;
  *(IDL_INT *) argv[3] = (IDL_INT) info.manualSupported;
  *(IDL_INT *) argv[4] = (IDL_INT) info.onOffSupported;
  *(IDL_INT *) argv[5] = (IDL_INT) info.absValSupported;
  *(IDL_INT *) argv[6] = (IDL_INT) info.readOutSupported;
  *(IDL_ULONG *) argv[7] = (IDL_ULONG) info.min;
  *(IDL_ULONG *) argv[8] = (IDL_ULONG) info.max;
  *(float *) argv[9] = info.absMin;
  *(float *) argv[10] = info.absMax;

  return (IDL_INT) error;
}

//
// READ_PROPERTY
//
// Read property values from camera
//
// Reference: FlyCapture2Defs_C.h
//
// argv[0]: IN property type
// argv[1]: OUT present
// argv[2]: OUT absControl
// argv[3]: OUT onePush
// argv[4]: OUT onOff
// argv[5]: OUT autoManualMode
// argv[6]: OUT valueA
// argv[7]: OUT valueB
// argv[8]: OUT absValue
//
IDL_INT IDL_CDECL read_property(int argc, char *argv[])
{
  fc2Error error;
  fc2Property property;

  property.type = *(fc2PropertyType *) argv[0];
  error = fc2GetProperty(context, &property);

  *(IDL_INT *) argv[1] = (IDL_INT) property.present;
  *(IDL_INT *) argv[2] = (IDL_INT) property.absControl;
  *(IDL_INT *) argv[3] = (IDL_INT) property.onePush;
  *(IDL_INT *) argv[4] = (IDL_INT) property.onOff;
  *(IDL_INT *) argv[5] = (IDL_INT) property.autoManualMode;
  *(IDL_ULONG *) argv[6] = (IDL_ULONG) property.valueA;
  *(IDL_ULONG *) argv[7] = (IDL_ULONG) property.valueB;
  *(float *) argv[8] = property.absValue;

  return (IDL_INT) error;
}

//
// WRITE_PROPERTY
//
// Write property values to camera
//
// Reference: FlyCapture2Defs_C.h
//
// argv[0]: IN property type
// argv[1]: IN absControl -- floating point value
// argv[2]: IN onePush -- camera controls feature once and then returns control
// argv[3]: IN onOFF -- true means property is on
// argv[4]: IN autoManualMode -- true means property is set automatically
// argv[5]: IN valueA
// argv[6]: IN valueB not used?
// argv[7]: IN absValue -- floating point value if absControl is true
//
IDL_INT IDL_CDECL write_property(int argc, char *argv[])
{
  fc2Error error;
  fc2Property property;

  property.type = *(fc2PropertyType *) argv[0];
  property.absControl = *(BOOL *) argv[1];
  property.onePush = *(BOOL *) argv[2];
  property.onOff = *(BOOL *) argv[3];
  property.autoManualMode = *(BOOL *) argv[4];
  property.valueA = *(unsigned int *) argv[5];
  property.valueB = *(unsigned int *) argv[6];
  property.absValue = *(float *) argv[7];
 
  error = fc2SetProperty(context, &property);

  return (IDL_INT) error;
}

  
//
// IDL_Load
//
int IDL_Load (void)
{
  int status;
  int nmsgs, nfcns, npros;

  static IDL_SYSFUN_DEF2 function_addr[] = {
    { idlpgr_CreateContext, "IDLPGR_CREATECONTEXT", 0, 0, 0, 0 },
    { idlpgr_GetNumOfCameras, "IDLPGR_GETNUMOFCAMERAS", 1, 1, 0, 0 },
    { idlpgr_GetCameraFromIndex, "IDLPGR_GETCAMERAFROMINDEX", 1, 2, 0, 0 },
  };

  static IDL_SYSFUN_DEF2 procedure_addr[] = {
    { (IDL_SYSRTN_GENERIC)
      idlpgr_DestroyContext, "IDLPGR_DESTROYCONTEXT", 1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_Connect, "IDLPGR_CONNECT", 2, 2, 0, 0 },
  };

  nmsgs = IDL_CARRAY_ELTS(msg_arr);
  msgs = IDL_MessageDefineBlock("idlpgr", nmsgs, msg_arr);
  if (!msgs)
    return IDL_FALSE;

  nfcns = IDL_CARRAY_ELTS(function_addr);
  status = IDL_SysRtnAdd(function_addr, TRUE, nfcns);

  npros = IDL_CARRAY_ELTS(procedure_addr);
  status &= IDL_SysRtnAdd(procedure_addr, FALSE, npros);

  return status;
}
