#include "SimpliedDecodeClass.h"
#include <stdio.h>
//Windows
extern "C"
{
	#include "libavcodec/avcodec.h"
	#include "libswscale/swscale.h"
};
#ifdef WIN32
#include <windows.h>
#include <process.h>  
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int DecodedFrameFree(SimpliedDecodedFrame* pFrame);
unsigned int  __stdcall DecodeInputStreamThreadFunc(void* pParam);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define			USE_SWS_SCALE 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
CSimpliedDecode::CSimpliedDecode(DECODE_FORMAT streamFormat,DECODED_PIXEL_FORMAT pixelFromat)
{
	this->streamFormat  = streamFormat;
	this->pixelFromat  = pixelFromat;
	m_pCodec = NULL;
	m_pCodecCtx = NULL;
	m_pCodecParserCtx = NULL;
	if(streamFormat == DECODE_FORMAT_H264)
	{
		m_codec_id	=	(int)AV_CODEC_ID_H264;
	}
	else
	{
		m_codec_id    = 0;
	}
	decFuncInfCnt = 0;
	m_hEvent = NULL;
	m_hLock = NULL;
	m_hThread = NULL;
	m_bThreadQuit = false;
}
CSimpliedDecode::~CSimpliedDecode()
{
	
}

int CSimpliedDecode::addDecodeFuncInf(PDecodedDataCallBack pFunc,void* pUserData)
{
	SimpliedDecodeFuncInf decFuncInf;
	decFuncInf.pFunc = pFunc;
	decFuncInf.pUserData = pUserData;
	lock();
	int funcFlag = decFuncInfCnt++;
	unlock();
	mapDecFuncInf.insert(make_pair(funcFlag,decFuncInf));
	return funcFlag;
}
int CSimpliedDecode::delDecodeFuncInf(int funcFlag)
{
	map<int,SimpliedDecodeFuncInf>::iterator mapIter ;
	mapIter = mapDecFuncInf.find(funcFlag);
	if(mapIter != mapDecFuncInf.end())
	{
		mapDecFuncInf.erase(funcFlag);
		return 0;
	}
	return -1;
}
int	CSimpliedDecode::decodeStream(void* pInputData,int nInputDataLen,const char* pVer,void* pDecOutData,int* pDecOutDataLen)
{
	if(pInputData == NULL)
	{
		return -1;
	}
#if 0
	static FILE* fTestOutputH264 = fopen("d:\\test_20161024_yuv.yuv","ab");
	if(fTestOutputH264 != NULL)
	{
		fwrite(pInputData,1,nInputDataLen,fTestOutputH264);
	}
#endif
	int cur_size = 0;
	//int nAvParseCounts = 0;
	uint8_t* cur_ptr = (uint8_t*)pInputData;
	cur_size = nInputDataLen;
	AVPacket packet;
	av_init_packet(&packet);
	AVFrame* pFrame = av_frame_alloc();
	AVCodecContext* pCodecCtx = (AVCodecContext*)m_pCodecCtx;
	AVCodecParserContext* pCodecParserCtx = (AVCodecParserContext*)m_pCodecParserCtx;
	
	
#if 0
	char* pChBuf = (char*)pInputData;
	if(pChBuf[0] == 0 && pChBuf[1] == 0 && pChBuf[2] == 0 && pChBuf[3] == 1)
	{
		//short shValue = 0;
		//memcpy(&shValue,&pChBuf[4],sizeof(shValue));
		unsigned char uch8Value = pChBuf[4];
		if((uch8Value & 0x1f) == 0x5)
		{
			printf("i frame \n");
		}
	}
	else if(pChBuf[0] == 0 && pChBuf[1] == 0 && pChBuf[2] == 0)
	{
		unsigned char uch8Value = pChBuf[3];
		if((uch8Value & 0x1f) == 0x5)
		{
			printf("i frame \n");
		}
	}
#endif

	while (cur_size>0)
	{
		 int len = av_parser_parse2(pCodecParserCtx,pCodecCtx,&packet.data, &packet.size,
				cur_ptr , cur_size ,AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
			if(len < 0)
			{
				printf("av_parser_parse2 error:%d in %s \n",len,__FUNCTION__);
				break;
			}
#if 0
			else if(len == 0)
			{
				//连续检测10次都没有可解码帧则退出
				if(nAvParseCounts++ > 10)
				{
					//break;
				}
			}
			else
			{
				nAvParseCounts = 0;
			}
#endif
			cur_ptr += len;
			cur_size -= len;

			if(packet.size==0)
				continue;

			//Some Info from AVCodecParserContext
			printf("[Packet]Size:%6d\t",packet.size);
			switch(((AVCodecParserContext*)(pCodecParserCtx))->pict_type)
			{
				case AV_PICTURE_TYPE_I: printf("Type:I\t");break;
				case AV_PICTURE_TYPE_P: printf("Type:P\t");break;
				case AV_PICTURE_TYPE_B: printf("Type:B\t");break;
				default: printf("Type:Other\t");break;
			}
			printf("Number:%4d\n",((AVCodecParserContext*)pCodecParserCtx)->output_picture_number);
			int decode_ok = 0;
		#if 1
			int ret = avcodec_decode_video2((AVCodecContext*)pCodecCtx, pFrame, &decode_ok, &packet);
			if (ret < 0) 
			{
				printf("Decode Error.\n");
				av_frame_free(&pFrame);
				return ret;
			}
		#endif
			if(decode_ok)
			{
				printf("\nCodec Full Name:%s\n",pCodecCtx->codec->long_name);
				printf("width:%d\nheight:%d\n\n",pCodecCtx->width,pCodecCtx->height);
				decodedFrameNotify((void*)pCodecCtx,(void*)pFrame);
			}
	}
	//flush decoder 
	//avcodec_decode_video2 returned picture frame use   
	//av_frame_alloc() to get an AVFrame. The codec will 
	//allocate memory for the actual bitmap by calling the 
	//AVCodecContext.get_buffer2() callback. 
	//When AVCodecContext.refcounted_frames is set to 1, the frame is 
	//reference counted and the returned reference belongs to the 
	//caller. The caller must release the frame using av_frame_unref() 
	//when the frame is no longer needed. The caller may safely write 
	//to the frame if av_frame_is_writable() returns 1. 
	//When AVCodecContext.refcounted_frames is set to 0, the returned 
	//reference belongs to the decoder and is valid only until the 
	//next call to this function or until closing or flushing the 
	//decoder. The caller may not write to it. 
	packet.data = NULL;
	packet.size = 0;
	while(1)
	{
		int decode_ok = 0;
		int ret = avcodec_decode_video2((AVCodecContext*)pCodecCtx, pFrame, &decode_ok, &packet);
		if (ret < 0) 
		{
			printf("Decode Error.\n");
			av_frame_free(&pFrame);
			return ret;
		}
		if(decode_ok == 0)
		{
			break;
		}
		else if(decode_ok > 0)
		{
			decodedFrameNotify((void*)pCodecCtx,(void*)pFrame);
		}
	}
	av_frame_free(&pFrame);
	return 0;
}
int CSimpliedDecode::inputStream(void* pInputData,int nInputDataLen)
{
	if(pInputData == NULL || nInputDataLen <= 0)
	{
		return -1;
	}
	void* pStoreData = malloc(nInputDataLen);
	if(pStoreData == NULL)
	{
		return -1;
	}
	//拷贝
	memcpy(pStoreData,pInputData,nInputDataLen);

	//加锁
	lock();

	SimpliedStreamElem ssElem;
	ssElem.pData = pStoreData;
	ssElem.nDataLen = nInputDataLen;
	//入队列
	m_listStreamElem.push_back(ssElem);
	//over 100 remove first
	if(m_listStreamElem.size() > 100)
	{
		SimpliedStreamElem ssPopElem;
		ssPopElem = m_listStreamElem.front();
		m_listStreamElem.pop_front();
		if(ssPopElem.pData != NULL)
		{
			free(ssPopElem.pData);
			ssPopElem.pData = NULL;
		}
	}
	//解锁
	unlock();

	//通知
	signalEvent();

	return 0;
}
int CSimpliedDecode::start()
{
	createEvent();
	createLock();
	createThread(DecodeInputStreamThreadFunc);

	avcodec_register_all();

    m_pCodec = (void*)avcodec_find_decoder((AVCodecID)m_codec_id);
    if (!m_pCodec) 
	{
        printf("Codec not found\n");
        return -1;
    }
    m_pCodecCtx = (void*)avcodec_alloc_context3((AVCodec*)m_pCodec);
    if (!m_pCodecCtx)
	{
        printf("Could not allocate video codec context\n");
        return -1;
    }

	m_pCodecParserCtx= (void*)av_parser_init(m_codec_id);
	if (!m_pCodecParserCtx)
	{
		printf("Could not allocate video parser context\n");
		return -1;
	}
    
    if (avcodec_open2((AVCodecContext*)m_pCodecCtx, (AVCodec*)m_pCodec, NULL) < 0)
	{
        printf("Could not open codec\n");
        return -1;
    }
	return 0;
}
int CSimpliedDecode::stop()
{
	AVCodecContext* pCodecCtx = (AVCodecContext*)m_pCodecCtx;
	//AVCodec* pCodec =    (AVCodec*)m_pCodec;
	AVCodecParserContext* pCodecParserCtx = (AVCodecParserContext*)m_pCodecParserCtx;
	if(pCodecParserCtx != NULL)
	{
		av_parser_close((AVCodecParserContext*)pCodecParserCtx);
		pCodecParserCtx = NULL;
		m_pCodecParserCtx = NULL;
	}
	if(pCodecCtx != NULL)
	{
		avcodec_close((AVCodecContext*)pCodecCtx);
		av_free(pCodecCtx);
		pCodecCtx = NULL;
		m_pCodecCtx = NULL;
	}
	destroyThread();
	destroyStreamList();
	destroyEvent();
	destroyLock();
	
	return 0;
}
int CSimpliedDecode::createEvent()
{
	#ifdef WIN32
		m_hEvent = (void*)CreateEvent(NULL,0,0,"SimpliedDecodeEvent");
	#endif
	return 0;
}
int CSimpliedDecode::waitEvent()
{
	#ifdef WIN32
		if(m_hEvent != NULL)
		{
			WaitForSingleObject(m_hEvent,INFINITE);
		}
	#endif
	return 0;
}
int CSimpliedDecode::signalEvent()
{
	#ifdef WIN32
		if(m_hEvent != NULL)
		{
			SetEvent(m_hEvent);
		}
	#endif
	return 0;
}
int CSimpliedDecode::destroyEvent()
{
	#ifdef WIN32
		if(m_hEvent != NULL)
		{
			CloseHandle(m_hEvent);
			m_hEvent = NULL;
		}
	#endif
	return 0;
}
int CSimpliedDecode::createLock()
{
	#ifdef WIN32
		m_hLock = (void*)malloc(sizeof(CRITICAL_SECTION));
		InitializeCriticalSection((LPCRITICAL_SECTION)m_hLock);
	#endif
	return 0;
}
int CSimpliedDecode::destroyLock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			DeleteCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}
int CSimpliedDecode::lock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			EnterCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}
int CSimpliedDecode::unlock()
{
	#ifdef WIN32
		if(m_hLock != NULL)
		{
			LeaveCriticalSection((LPCRITICAL_SECTION)m_hLock);
		}
	#endif
	return 0;
}


int CSimpliedDecode::createThread(unsigned int (__stdcall *pThreadFunc)(void* pParam))
{
	#ifdef WIN32
		m_hThread = (void*)_beginthreadex(NULL,0,pThreadFunc,this,0,NULL);
	#endif
	return 0;
}
int CSimpliedDecode::destroyThread()
{
	#ifdef WIN32
		if(m_hThread != NULL)
		{
			m_bThreadQuit = true;
		}
	#endif
	return 0;
}
int  CSimpliedDecode::exitThread()
{
	#ifdef WIN32
		if(m_hThread != NULL)
		{
			_endthreadex(0);
			m_hThread = NULL;
		}
	#endif
	return 0;
}
bool CSimpliedDecode::isThreadQuit()
{
	bool bQuit = false;	
	lock();
	bQuit = m_bThreadQuit;
	unlock();
	return bQuit;
}
unsigned int CSimpliedDecode::getStreamListSize()
{
	unsigned int uListSize = 0;
	lock();
	uListSize = m_listStreamElem.size();
	unlock();
	return uListSize;
}
int CSimpliedDecode::popListStreamElemFront(SimpliedStreamElem* pElem)
{
	if(pElem == NULL)
	{
		return -1;
	}
	lock();
	*pElem = m_listStreamElem.front();
	m_listStreamElem.pop_front();
	unlock();
	return 0;
}
int CSimpliedDecode::destroyStreamList()
{
	lock();
	list<SimpliedStreamElem>::iterator listIter = m_listStreamElem.begin();
	for(;listIter != m_listStreamElem.end();listIter++)
	{
		if(listIter->pData != NULL)
		{
			free(listIter->pData);
			listIter->pData  = NULL;
			listIter->nDataLen = 0;
		}
	}
	m_listStreamElem.clear();
	unlock();
	return 0;
}
int CSimpliedDecode::decodedFrameNotify(void* pCtx,void* pFrm)
{
	if(pCtx == NULL || pFrm == NULL)
	{
		return -1;
	}
	AVCodecContext* pCodecCtx = (AVCodecContext*)pCtx;
	AVFrame* pFrame  = (AVFrame*) pFrm; 
	#ifdef USE_SWS_SCALE
		//SwsContext
		struct SwsContext *img_convert_ctx = NULL;
		AVFrame	*pFrameYUV = NULL;
		uint8_t *out_buffer = NULL;
		img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
				pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 

		pFrameYUV=av_frame_alloc();
		out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
		avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

		int y_size=pCodecCtx->width*pCodecCtx->height;
		int outHeight = sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
			pFrameYUV->data, pFrameYUV->linesize);

		printf("sws_scale out height:%d\n",outHeight);
	
		map<int,SimpliedDecodeFuncInf>::iterator mapIter = mapDecFuncInf.begin() ;
				
		for(;mapIter != mapDecFuncInf.end();mapIter++)
		{
			if(mapIter->second.pFunc != NULL)
			{
				SimpliedDecodedDataVer1 dataVer1;
				memset(&dataVer1,0,sizeof(dataVer1));
				dataVer1.decodedFrame.width = pCodecCtx->width;
				dataVer1.decodedFrame.height = pCodecCtx->height;
				dataVer1.decodedFrame.pixelFormat = DECODE_FORMAT_YUV_420P;
				dataVer1.decodedFrame.pChl[0] = pFrameYUV->data[0];
				dataVer1.decodedFrame.pChl[1] = pFrameYUV->data[1];
				dataVer1.decodedFrame.pChl[2] = pFrameYUV->data[2];
				dataVer1.decodedFrame.chlLineSize[0] =  pFrameYUV->linesize[0];
				dataVer1.decodedFrame.chlLineSize[1] =  pFrameYUV->linesize[1];
				dataVer1.decodedFrame.chlLineSize[2] =  pFrameYUV->linesize[2];
				dataVer1.decodedFrame.chlLen[0] = pFrameYUV->linesize[0]*dataVer1.decodedFrame.height;
				dataVer1.decodedFrame.chlLen[1] = pFrameYUV->linesize[1]*dataVer1.decodedFrame.height;
				dataVer1.decodedFrame.chlLen[2] = pFrameYUV->linesize[2]*dataVer1.decodedFrame.height;
				dataVer1.userFreeFrame = 0;
				dataVer1.freeFrameFunc = NULL;
				mapIter->second.pFunc("V0.1",&dataVer1,sizeof(dataVer1),mapIter->second.pUserData);
			}
		}
		av_frame_free(&pFrameYUV);
		pFrameYUV = NULL;
		av_free(out_buffer);
		out_buffer = NULL;
		sws_freeContext(img_convert_ctx);
		img_convert_ctx = NULL;
	#else
		map<int,SimpliedDecodeFuncInf>::iterator mapIter = mapDecFuncInf.begin() ;
				
		for(;mapIter != mapDecFuncInf.end();mapIter++)
		{
			if(mapIter->second.pFunc != NULL)
			{
				SimpliedDecodedDataVer1 dataVer1;
				memset(&dataVer1,0,sizeof(dataVer1));
				dataVer1.decodedFrame.width = pCodecCtx->width;
				dataVer1.decodedFrame.height = pCodecCtx->height;
				dataVer1.decodedFrame.pixelFormat = DECODE_FORMAT_YUV_420P;
				dataVer1.decodedFrame.pChl[0] = pFrame->data[0];
				dataVer1.decodedFrame.pChl[1] = pFrame->data[1];
				dataVer1.decodedFrame.pChl[2] = pFrame->data[2];
				dataVer1.decodedFrame.chlLineSize[0] =  pFrame->linesize[0];
				dataVer1.decodedFrame.chlLineSize[1] =  pFrame->linesize[1];
				dataVer1.decodedFrame.chlLineSize[2] =  pFrame->linesize[2];
				dataVer1.decodedFrame.chlLen[0] = pFrame->linesize[0]*dataVer1.decodedFrame.height;
				dataVer1.decodedFrame.chlLen[1] = pFrame->linesize[1]*dataVer1.decodedFrame.height;
				dataVer1.decodedFrame.chlLen[2] = pFrame->linesize[2]*dataVer1.decodedFrame.height;
				dataVer1.userFreeFrame = 0;
				dataVer1.freeFrameFunc = NULL;
				mapIter->second.pFunc("V0.1",&dataVer1,sizeof(dataVer1),mapIter->second.pUserData);
			}
		}
	#endif
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
int DecodedFrameFree(SimpliedDecodedFrame* pFrame)
{
	if(pFrame == NULL)
	{
		return -1;
	}
	if(pFrame->pixelFormat == DECODE_FORMAT_YUV_420P)
	{
		av_free(pFrame->pChl[0]);
		pFrame->pChl[0] = NULL;
		pFrame->pChl[1] = NULL;
		pFrame->pChl[2] = NULL;
		pFrame->chlLen[0] = 0;
		pFrame->chlLen[1] = 0;
		pFrame->chlLen[2] = 0;
		pFrame->chlLineSize[0] = 0;
		pFrame->chlLineSize[1] = 0;
		pFrame->chlLineSize[2] = 0;
	}
	return 0;
}

unsigned int __stdcall  DecodeInputStreamThreadFunc(void* pParam)
{
	CSimpliedDecode* pDec = (CSimpliedDecode*)pParam;
	while(pDec != NULL && !pDec->isThreadQuit())
	{
		unsigned  listSize = 0;
		SimpliedStreamElem ssElem;
		memset(&ssElem,0,sizeof(ssElem));
		int popStreamOk = 0;
		int status = 0;
		//等待事件
		pDec->waitEvent();
		do
		{
			listSize = pDec->getStreamListSize();
			if(listSize == 0)
			{
				break;
			}
			status = pDec->popListStreamElemFront(&ssElem);
			if(status == 0)
			{
				popStreamOk = 1;
			}
			if(popStreamOk)
			{
				pDec->decodeStream(ssElem.pData,ssElem.nDataLen,NULL,NULL,NULL);
				if(ssElem.pData != NULL)
				{
					free(ssElem.pData);
					ssElem.pData = NULL;
					ssElem.nDataLen = 0;
				}
			}
		}while(listSize > 0);
	}while(1);
	pDec->exitThread();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////