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

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  error = fc2DestroyContext(context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not destroy specified context.", error);
}

//
// idlpgr_GetNumOfCameras
//
IDL_VPTR idlpgr_GetNumOfCameras(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  unsigned int ncameras = 0;

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

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  camera = (argc == 2) ? (int) IDL_LongScalar(argv[1]) : 0;

  error = fc2GetCameraFromIndex(context, camera, &guid);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not acquire specified camera.", error);

  // transfer camera guid to IDL as a vector of ULONG values
  pd = (IDL_ULONG *) 
    IDL_MakeTempVector(IDL_TYP_ULONG, dim, IDL_ARR_INI_NOP, &idl_guid);
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
  
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

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
// idlpgr_StartCapture
//
void idlpgr_StartCapture(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  
  error = fc2StartCapture(context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not start capture.", error);
}

//
// idlpgr_StopCapture
//
void idlpgr_StopCapture(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  
  error = fc2StopCapture(context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not stop capture.", error);
}

//
// idlpgr_CreateImage
//
IDL_VPTR idlpgr_CreateImage(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2Image image;
  IDL_MEMINT dim;
  unsigned char *pd;
  IDL_VPTR idl_image;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  error = fc2CreateImage(&image);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could create image.", error);

  dim = (IDL_MEMINT) sizeof(image);
  
  pd = IDL_MakeTempVector(IDL_TYP_BYTE, dim, IDL_ARR_INI_ZERO, &idl_image);
  memcpy(pd, (unsigned char *) &image, dim);

  return idl_image;
}
//
// idlpgr_DestroyImage
//
void idlpgr_DestroyImage(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Image *image;

  IDL_ENSURE_ARRAY(argv[0]);
  if (argv[0]->value.arr->n_elts != sizeof(fc2Image))
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERROR, IDL_MSG_LONGJMP,
			 "Argument is not a valid image descriptor.");
  
  image = (fc2Image *) argv[0]->value.arr->data;

  error = fc2DestroyImage(image);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not destroy image.", error);
}

//
// idlpgr_RetrieveBuffer
//
IDL_VPTR idlpgr_RetrieveBuffer(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2Image *image;
  IDL_MEMINT ndims, dims[3];

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  IDL_ENSURE_ARRAY(argv[1]);
  if (argv[1]->value.arr->n_elts != sizeof(fc2Image))
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERROR, IDL_MSG_LONGJMP,
			 "Argument is not a valid image descriptor.");
  image = (fc2Image *) argv[1]->value.arr->data;
  
  error = fc2RetrieveBuffer(context, image);
  if (error) 
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not retrieve image buffer.", error);

  if (image->cols == image->stride) {
    ndims = 2;
    dims[0] = image->cols;
    dims[1] = image->rows;
  } else {
    ndims = 3;
    dims[0] = 3;
    dims[1] = image->cols;
    dims[2] = image->rows;
  }
  
  return IDL_ImportArray(ndims, dims, IDL_TYP_BYTE,
			 (UCHAR *) image->pData, NULL, NULL);
}

//
// idlpgr_ReadRegister
//
// Read contents of specified register
//
IDL_VPTR idlpgr_ReadRegister(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  unsigned int address, value;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  address = (unsigned int) IDL_ULongScalar(argv[1]);
  
  error = fc2ReadRegister(context, address, &value);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not read from specified register.", error);
  
  return IDL_GettmpULong((IDL_ULONG) value);
}

//
// idlpgr_WriteRegister
//
// Write unsigned integer to specified register
//
void idlpgr_WriteRegister(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  unsigned int address, value;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  address = (unsigned int) IDL_ULongScalar(argv[1]);
  value =   (unsigned int) IDL_ULongScalar(argv[2]);

  error = fc2WriteRegister(context, address, value);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not write value to specified register." error);
}

//
// idlpgr_GetPropertyInfo
//
// Get information about property
//
// Reference: FlyCapture2Defs_C.h
//
IDL_VPTR idlpgr_GetPropertyInfo(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2PropertyInfo info;
  IDL_MEMINT s[] = {1, MAX_STRING_LENGTH};
  IDL_MEMINT r[] = {1, 8};
  void *idl_info;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  info.type = (fc2PropertyType) IDL_Long(argv[1]);

  error = fc2GetPropertyInfo(context, &info);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORCODE, IDL_MSG_LONGJMP,
			 "Could not get requested property information.", error);
  
  static IDL_STRUCT_TAG_DEF tags[] = {
    { "PRESENT",          0, (void *) IDL_TYP_LONG },
    { "AUTOSUPPORTED",    0, (void *) IDL_TYP_LONG },
    { "MANUALSUPPORTED",  0, (void *) IDL_TYP_LONG },
    { "ONOFFSUPPORTED",   0, (void *) IDL_TYP_LONG },
    { "ABSVALSUPPORTED",  0, (void *) IDL_TYP_LONG },
    { "READOUTSUPPORTED", 0, (void *) IDL_TYP_LONG },
    { "MIN",              0, (void *) IDL_TYP_ULONG },
    { "MAX",              0, (void *) IDL_TYP_ULONG },
    { "ABSMIN",           0, (void *) IDL_TYP_FLOAT },
    { "ABSMAX",           0, (void *) IDL_TYP_FLOAT },
    { "PUNITS",           s, (void *) IDL_TYP_STRING },
    { "PUNITABBR",        s, (void *) IDL_TYP_STRING },
    { "RESERVED",         r, (void *) IDL_TYP_ULONG },
    { 0 },
  };
  idl_info = IDL_MakeStruct("fc2PropertyInfo", tags);
  memcpy((char *) idl_info->value.s.arr->data, (char *) &info, sizeof(fc2PropertyInfo));

  return idl_info;
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
    { idlpgr_CreateImage, "IDLPGR_CREATEIMAGE", 1, 1, 0, 0 },
    { idlpgr_RetrieveBuffer, "IDLPGR_RETRIEVEBUFFER", 2, 2, 0, 0 },
    { idlpgr_ReadRegister, "IDLPGR_READREGISTER", 2, 2, 0, 0 },
    { idlpgr_GetPropertyInfo, "IDLPGR_GETPROPERTYINFO", 2, 2, 0, 0 },
  };

  static IDL_SYSFUN_DEF2 procedure_addr[] = {
    { (IDL_SYSRTN_GENERIC)
      idlpgr_DestroyContext, "IDLPGR_DESTROYCONTEXT", 1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_Connect, "IDLPGR_CONNECT", 2, 2, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_StartCapture, "IDLPGR_STARTCAPTURE", 1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_StopCapture, "IDLPGR_STOPCAPTURE", 1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_DestroyImage, "IDLPGR_DESTROYIMAGE", 1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_WriteRegister, "IDLPGR_WRITEREGISTER", 3, 3, 0, 0 },
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
