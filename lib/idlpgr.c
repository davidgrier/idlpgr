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

// Error messages
static IDL_MSG_DEF msg_arr[] =
  {
#define M_IDLPGR_ERROR      0
    { "M_IDLPGR_ERROR", "%NError: %s" },
#define M_IDLPGR_ERRORCODE -1
    { "M_IDLPGR_ERRORCODE", "%NError: %s Code: %0X" },
#define M_IDLPGR_ERRORSTRING -2
    { "M_IDLPGR_ERRORSTRING", "%NError: %s: %s" },
  };

static IDL_MSG_BLOCK msgs;

//
// idlpgr_CreateContext
//
IDL_VPTR IDL_CDECL idlpgr_CreateContext(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;

  error = fc2CreateContext(&context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP, 
			 "Could not create context",
			 fc2ErrorToDescription(error));

  return IDL_GettmpULong64((IDL_ULONG64) context);
}

//
// idlpgr_DestroyContext
//
void IDL_CDECL idlpgr_DestroyContext(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  error = fc2DestroyContext(context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not destroy specified context",
			 fc2ErrorToDescription(error));
}

//
// idlpgr_GetNumOfCameras
//
IDL_VPTR IDL_CDECL idlpgr_GetNumOfCameras(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  unsigned int ncameras = 0;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  error = fc2GetNumOfCameras(context, &ncameras);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not count cameras",
			 fc2ErrorToDescription(error));
  
  return IDL_GettmpLong(ncameras);
}

//
// idlpgr_GetCameraFromIndex
//
IDL_VPTR IDL_CDECL idlpgr_GetCameraFromIndex(int argc, IDL_VPTR argv[])
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
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not acquire specified camera",
			 fc2ErrorToDescription(error));

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
void IDL_CDECL idlpgr_Connect(int argc, IDL_VPTR argv[])
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
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not connect camera to context",
			 fc2ErrorToDescription(error));
}

//
// idlpgr_GetCameraInfo
//
// Returns a subset of the fc2CameraInfo structure
//
IDL_VPTR IDL_CDECL idlpgr_GetCameraInfo(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2CameraInfo camerainfo;
  IDL_StructDefPtr sdef;
  char *pd;
  IDL_MEMINT one = 1;
  IDL_VPTR idl_camerainfo;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  error = fc2GetCameraInfo(context, &camerainfo);
  if (error) 
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not read camera info",
			 fc2ErrorToDescription(error));

  static IDL_STRUCT_TAG_DEF tags[] = {
    { "SERIALNUMBER",     0, (void *) IDL_TYP_ULONG },
    { "ISCOLORCAMERA",    0, (void *) IDL_TYP_LONG },
    { "MODELNAME",        0, (void *) IDL_TYP_STRING },
    { "VENDORNAME",       0, (void *) IDL_TYP_STRING },
    { "SENSORINFO",       0, (void *) IDL_TYP_STRING },
    { "SENSORRESOLUTION", 0, (void *) IDL_TYP_STRING },
    { "DRIVERNAME",       0, (void *) IDL_TYP_STRING },
    { "FIRMWAREVERSION",  0, (void *) IDL_TYP_STRING },
    { 0 }
  };
  sdef = IDL_MakeStruct("fc2CameraInfo", tags);
  pd = IDL_MakeTempStruct(sdef, 1, &one, &idl_camerainfo, TRUE);
  *(IDL_ULONG *) pd      = camerainfo.serialNumber;
  pd += sizeof(IDL_ULONG);
  *(IDL_LONG *) pd = camerainfo.isColorCamera;
  pd += sizeof(IDL_LONG);
  IDL_StrStore((IDL_STRING *) pd, camerainfo.modelName);
  pd += sizeof(IDL_STRING);
  IDL_StrStore((IDL_STRING *) pd, camerainfo.vendorName);
  pd += sizeof(IDL_STRING);
  IDL_StrStore((IDL_STRING *) pd, camerainfo.sensorInfo);
  pd += sizeof(IDL_STRING);
  IDL_StrStore((IDL_STRING *) pd, camerainfo.sensorResolution);
  pd += sizeof(IDL_STRING);
  IDL_StrStore((IDL_STRING *) pd, camerainfo.driverName);
  pd += sizeof(IDL_STRING);
  IDL_StrStore((IDL_STRING *) pd, camerainfo.firmwareVersion);
  
  return idl_camerainfo;
}

//
// idlpgr_StartCapture
//
void IDL_CDECL idlpgr_StartCapture(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  
  error = fc2StartCapture(context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not start capture",
			 fc2ErrorToDescription(error));
}

//
// idlpgr_StopCapture
//
void IDL_CDECL idlpgr_StopCapture(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  
  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  
  error = fc2StopCapture(context);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not stop capture",
			 fc2ErrorToDescription(error));
}

//
// idlpgr_CreateImage
//
IDL_VPTR IDL_CDECL idlpgr_CreateImage(int argc, IDL_VPTR argv[])
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
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could create image",
			 fc2ErrorToDescription(error));

  dim = (IDL_MEMINT) sizeof(image);
  
  pd = IDL_MakeTempVector(IDL_TYP_BYTE, dim, IDL_ARR_INI_ZERO, &idl_image);
  memcpy(pd, (unsigned char *) &image, dim);

  return idl_image;
}

//
// idlpgr_DestroyImage
//
void IDL_CDECL idlpgr_DestroyImage(int argc, IDL_VPTR argv[])
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
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not destroy image",
			 fc2ErrorToDescription(error));
}

//
// idlpgr_RetrieveBuffer
//
IDL_VPTR IDL_CDECL idlpgr_RetrieveBuffer(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2Image *image;
  IDL_MEMINT ndims, dims[3];
  IDL_VPTR idl_image;
  UCHAR *pd;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  IDL_ENSURE_ARRAY(argv[1]);
  if (argv[1]->value.arr->n_elts != sizeof(fc2Image))
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERROR, IDL_MSG_LONGJMP,
			 "Argument is not a valid image descriptor.");
  image = (fc2Image *) argv[1]->value.arr->data;
  
  error = fc2RetrieveBuffer(context, image);
  if (error) 
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not retrieve image buffer",
			 fc2ErrorToDescription(error));

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

  pd = (UCHAR *) IDL_MakeTempArray(IDL_TYP_BYTE, ndims, dims,
  				   IDL_ARR_INI_NOP, &idl_image);
  memcpy(pd, image->pData, image->rows*image->stride);
  return idl_image;
}

//
// idlpgr_ReadRegister
//
// Read contents of specified register
//
IDL_VPTR IDL_CDECL idlpgr_ReadRegister(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  unsigned int address, value;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  address = (unsigned int) IDL_ULongScalar(argv[1]);
  
  error = fc2ReadRegister(context, address, &value);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not read from specified register",
			 fc2ErrorToDescription(error));
  
  return IDL_GettmpULong((IDL_ULONG) value);
}

//
// idlpgr_WriteRegister
//
// Write unsigned integer to specified register
//
void IDL_CDECL idlpgr_WriteRegister(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  unsigned int address, value;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  address = (unsigned int) IDL_ULongScalar(argv[1]);
  value =   (unsigned int) IDL_ULongScalar(argv[2]);

  error = fc2WriteRegister(context, address, value);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not write value to specified register",
			 fc2ErrorToDescription(error));
}

//
// idlpgr_GetPropertyInfo
//
// Get information about property
//
// Reference: FlyCapture2Defs_C.h
//
IDL_VPTR IDL_CDECL idlpgr_GetPropertyInfo(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2PropertyInfo info;
  static IDL_MEMINT one = 1;
  static IDL_MEMINT r[] = {1, 8};
  static IDL_MEMINT s[] = {1, MAX_STRING_LENGTH};
  IDL_VPTR idl_info;
  IDL_StructDefPtr sdef;
  char *pd;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  info.type = (fc2PropertyType) IDL_LongScalar(argv[1]);

  error = fc2GetPropertyInfo(context, &info);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not get requested property information",
			 fc2ErrorToDescription(error));

  static IDL_STRUCT_TAG_DEF tags[] = {
    { "TYPE",             0, (void *) IDL_TYP_LONG },
    { "PRESENT",          0, (void *) IDL_TYP_LONG },
    { "AUTOSUPPORTED",    0, (void *) IDL_TYP_LONG },
    { "MANUALSUPPORTED",  0, (void *) IDL_TYP_LONG },
    { "ONOFFSUPPORTED",   0, (void *) IDL_TYP_LONG },
    { "ONEPUSHSUPPORTED", 0, (void *) IDL_TYP_LONG },
    { "ABSVALSUPPORTED",  0, (void *) IDL_TYP_LONG },
    { "READOUTSUPPORTED", 0, (void *) IDL_TYP_LONG },
    { "MIN",              0, (void *) IDL_TYP_ULONG },
    { "MAX",              0, (void *) IDL_TYP_ULONG },
    { "ABSMIN",           0, (void *) IDL_TYP_FLOAT },
    { "ABSMAX",           0, (void *) IDL_TYP_FLOAT },
    { "PUNITS",           s, (void *) IDL_TYP_BYTE },
    { "PUNITABBR",        s, (void *) IDL_TYP_BYTE },
    { "RESERVED",         r, (void *) IDL_TYP_ULONG },
    { 0 }
  };

  sdef = IDL_MakeStruct("fc2PropertyInfo", tags);
  pd = IDL_MakeTempStruct(sdef, 1, &one, &idl_info, TRUE);
  memcpy(pd, (char *) &info, sizeof(fc2PropertyInfo));

  return idl_info;
}

//
// idlpgr_GetProperty
//
// Read property values from camera
//
// Reference: FlyCapture2Defs_C.h
//
IDL_VPTR IDL_CDECL idlpgr_GetProperty(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2Property property;
  static IDL_MEMINT r[] = {1, 8};
  static IDL_MEMINT one = 1;
  IDL_VPTR idl_property;
  IDL_StructDefPtr sdef;
  char *pd;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);
  property.type = (fc2PropertyType) IDL_LongScalar(argv[1]);

  error = fc2GetProperty(context, &property);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not get requested property",
			 fc2ErrorToDescription(error));
  
  static IDL_STRUCT_TAG_DEF tags[] = {
    { "TYPE",           0, (void *) IDL_TYP_LONG },
    { "PRESENT",        0, (void *) IDL_TYP_LONG },
    { "ABSCONTROL",     0, (void *) IDL_TYP_LONG },
    { "ONEPUSH",        0, (void *) IDL_TYP_LONG },
    { "ONOFF",          0, (void *) IDL_TYP_LONG },
    { "AUTOMANUALMODE", 0, (void *) IDL_TYP_LONG },
    { "VALUEA",         0, (void *) IDL_TYP_ULONG },
    { "VALUEB",         0, (void *) IDL_TYP_ULONG },
    { "ABSVALUE",       0, (void *) IDL_TYP_FLOAT },
    { "RESERVED",       r, (void *) IDL_TYP_ULONG },
    { 0 }
  };
  sdef = IDL_MakeStruct("fc2Property", tags);
  pd = IDL_MakeTempStruct(sdef, 1, &one, &idl_property, TRUE);
  memcpy(pd, (char *) &property, sizeof(fc2Property));

  return idl_property;
}

//
// idlpgr_SetProperty
//
// Write property values to camera
//
// Reference: FlyCapture2Defs_C.h
//
void IDL_CDECL idlpgr_SetProperty(int argc, IDL_VPTR argv[])
{
  fc2Error error;
  fc2Context context;
  fc2Property property;
  char *sname;

  context = (fc2Context) IDL_ULong64Scalar(argv[0]);

  IDL_ENSURE_STRUCTURE(argv[1]);
  IDL_StructTagNameByIndex(argv[1]->value.s.sdef, 0, IDL_MSG_LONGJMP, &sname);
  if (strcmp(sname, "fc2Property"))
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERROR, IDL_MSG_LONGJMP,
			 "Argument is not of type fc2Property.");

  memcpy((char *) &property, (char *) argv[1]->value.s.arr->data, 
	 sizeof(fc2Property));

  error = fc2SetProperty(context, &property);
  if (error)
    IDL_MessageFromBlock(msgs, M_IDLPGR_ERRORSTRING, IDL_MSG_LONGJMP,
			 "Could not set requested property",
			 fc2ErrorToDescription(error));
}

//
// IDL_Load
//
int IDL_Load (void)
{
  int status;
  int nmsgs, nfcns, npros;

  static IDL_SYSFUN_DEF2 function_addr[] = {
    { idlpgr_CreateContext,      "IDLPGR_CREATECONTEXT",      0, 0, 0, 0 },
    { idlpgr_GetNumOfCameras,    "IDLPGR_GETNUMOFCAMERAS",    1, 1, 0, 0 },
    { idlpgr_GetCameraFromIndex, "IDLPGR_GETCAMERAFROMINDEX", 1, 2, 0, 0 },
    { idlpgr_GetCameraInfo,      "IDLPGR_GETCAMERAINFO",      1, 1, 0, 0 },
    { idlpgr_CreateImage,        "IDLPGR_CREATEIMAGE",        1, 1, 0, 0 },
    { idlpgr_RetrieveBuffer,     "IDLPGR_RETRIEVEBUFFER",     2, 2, 0, 0 },
    { idlpgr_ReadRegister,       "IDLPGR_READREGISTER",       2, 2, 0, 0 },
    { idlpgr_GetPropertyInfo,    "IDLPGR_GETPROPERTYINFO",    2, 2, 0, 0 },
    { idlpgr_GetProperty,        "IDLPGR_GETPROPERTY",        2, 2, 0, 0 },
  };

  static IDL_SYSFUN_DEF2 procedure_addr[] = {
    { (IDL_SYSRTN_GENERIC)
      idlpgr_DestroyContext, "IDLPGR_DESTROYCONTEXT", 1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_Connect,        "IDLPGR_CONNECT",        2, 2, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_StartCapture,   "IDLPGR_STARTCAPTURE",   1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_StopCapture,    "IDLPGR_STOPCAPTURE",    1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_DestroyImage,   "IDLPGR_DESTROYIMAGE",   1, 1, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_WriteRegister,  "IDLPGR_WRITEREGISTER",  3, 3, 0, 0 },
    { (IDL_SYSRTN_GENERIC)
      idlpgr_SetProperty,    "IDLPGR_SETPROPERTY",    2, 2, 0, 0 },
  };

  nmsgs = IDL_CARRAY_ELTS(msg_arr);
  msgs = IDL_MessageDefineBlock("idlpgr", nmsgs, msg_arr);
  if (!msgs)
    return IDL_FALSE;

  nfcns = IDL_CARRAY_ELTS(function_addr);
  status = IDL_SysRtnAdd(function_addr, TRUE, nfcns);

  npros = IDL_CARRAY_ELTS(procedure_addr);
  status |= IDL_SysRtnAdd(procedure_addr, FALSE, npros);

  return status;
}
