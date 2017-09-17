#include "SimpliedDecode.h"
#include "SimpliedDecodePriDef.h"
#include "SimpliedDecodeClass.h"
HSDecode 	CreateSimpliedDecode(DECODE_FORMAT streamFormat,DECODED_PIXEL_FORMAT pixelFormat)
{
	CSimpliedDecode* pDec = new CSimpliedDecode(streamFormat,pixelFormat);
	return pDec;
}
int	DestorySimpliedDecode(HSDecode hDecode)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)hDecode;
	if(pDec == NULL)
	{
		return -1;
	}
	delete pDec;
	return 0;
}

int	AddDecodeFunc(HSDecode hDecode,PDecodedDataCallBack pFunc,void* pUserData)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)hDecode;
	if(pDec == NULL)
	{
		return -1;
	}
	return pDec->addDecodeFuncInf(pFunc,pUserData);
}

int	DelDecodeFunc(HSDecode hDecode,int flag)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)hDecode;
	if(pDec == NULL)
	{
		return -1;
	}
	return pDec->delDecodeFuncInf(flag);
}

int	DecodeStream(HSDecode hDecode,void* pInputData,int nInputDataLen,const char* pVer,void* pDecOutData,int* pDecOutDataLen)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)hDecode;
	if(pDec == NULL)
	{
		return -1;
	}
	return pDec->decodeStream(pInputData,nInputDataLen,pVer,pDecOutData,pDecOutDataLen);
}
int  InputStream(HSDecode hDecode,void* pInputData,int nInputDataLen)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)hDecode;
	if(pDec == NULL)
	{
		return -1;
	}
	return pDec->inputStream(pInputData,nInputDataLen);
}
int	StartSimpliedDecode(HSDecode hDecode)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)hDecode;
	if(pDec == NULL)
	{
		return -1;
	}
	return pDec->start();
	
}

int	StopSimpliedDecode(HSDecode hDecode)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)hDecode;
	if(pDec == NULL)
	{
		return -1;
	}
	return pDec->stop();
}