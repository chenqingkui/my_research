#ifndef _SIMPLIED_DECODE_PRIVATE_DEF_H_
#define _SIMPLIED_DECODE_PRIVATE_DEF_H_
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef int (*PDecodedDataCallBack)(const char* pVer,const void* pData, const int nDataSize,const void* pUserData);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define	 MAX_DATA_CHL		8		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef  enum DECODE_FORMAT
{
	DECODE_FORMAT_H264    =  0x1,
}DECODE_FORMAT;

typedef enum DECODED_PIXEL_FORMAT
{
	DECODE_FORMAT_YUV_420P  = 0x1,
	DECODE_FORMAT_YV12		= 0x2,
	DECODE_FORMAT_RGB24		= 0x3,
}DECODED_PIXEL_FORMAT;

typedef struct SimpliedDecodeFuncInf
{
	PDecodedDataCallBack	 	pFunc;
	void* 						pUserData;
}SimpliedDecodeFuncInf,*PSimpliedDecodeFuncInf;

typedef struct SimpliedDecodedFrame
{
	DECODED_PIXEL_FORMAT pixelFormat;
	int 	width;
	int 	height;
	void*	pChl[MAX_DATA_CHL];
	int     chlLen[MAX_DATA_CHL];
	int		chlLineSize[MAX_DATA_CHL];
}SimpliedDecodedFrame;

//////////////////////////////////////////////////////////////////////////////////////
typedef int (*PDecodedFrameFreeFunc)(SimpliedDecodedFrame* pFrame);
/////////////////////////////////////////////////////////////////////////////////////
typedef struct SimpliedDecodedDataVer1
{
	SimpliedDecodedFrame		 decodedFrame;						//���������Frame��Ϣ
	int							 userFreeFrame;						//��ʾ�Ƿ���Ҫ�û��Լ��ͷ�Frame,Frame��Ӧ������
	PDecodedFrameFreeFunc		 freeFrameFunc;						//��ʾ�ͷ�Frame�ĺ���
}SimpliedDecodedDataVer1;
////////////////////////////////////////////////////////////////////////////////////
typedef void*					 HSDecode;
////////////////////////////////////////////////////////////////////////////////////
#endif