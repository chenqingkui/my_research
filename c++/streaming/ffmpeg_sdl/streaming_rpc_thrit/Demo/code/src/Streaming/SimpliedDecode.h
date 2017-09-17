#ifndef _SIMPLIED_DECODE_H_
#define _SIMPLIED_DECODE_H_
////////////////////////////////////////////////////////////////////////////////////
#include  "SimpliedDecodePriDef.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HSDecode 	 CreateSimpliedDecode(DECODE_FORMAT streamFormat,DECODED_PIXEL_FORMAT pixelFormat);

int			 AddDecodeFunc(HSDecode hDecode,PDecodedDataCallBack pFunc,void* pUserData);

int			 DelDecodeFunc(HSDecode hDecode,int flag);

int			 DestorySimpliedDecode(HSDecode hDecode);

int			 DecodeStream(HSDecode hDecode,void* pInputData,int nInputDataLen,const char* pVer,void* pDecOutData,int* pDecOutDataLen);


int          InputStream(HSDecode hDecode,void* pInputData,int nInputDataLen);

int			 StartSimpliedDecode(HSDecode hDecode);

int			 StopSimpliedDecode(HSDecode hDecode);
#endif