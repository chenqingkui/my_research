/** 
*作者：HJL 
*最后更新：2015.7.18 
*利用ffmpeg将RTSP传输的h264原始码流保存到文件中 
*未加任何效果，不显示 
**/  
  
  
#include <stdio.h>  
  
#define __STDC_CONSTANT_MACROS  
  
#ifdef _WIN32  
//Windows  
extern "C"  
{  
	#include "libavcodec/avcodec.h"  
	#include "libavformat/avformat.h"  
	#include "libswscale/swscale.h"  
	#include "SDL2/SDL.h"  
	#include "SDL2/SDL_thread.h"
};  
#else  
//Linux...  
#ifdef __cplusplus  
extern "C"  
{  
#endif  
#include <libavcodec/avcodec.h>  
#include <libavformat/avformat.h>  
#include <libswscale/swscale.h>  
#include <SDL2/SDL.h>  
#ifdef __cplusplus  
};  
#endif  
#endif 

#include "SimpliedStreamClass.h"

struct SimpliedRtspStreamFuncMeta
{
	PSimpliedStreamFunc pFunc;
	void*					pUserData1;
	void*					pUserData2;
};
struct SimpliedFFmpegMeta
{
	AVFormatContext* 	pFormatCtx;   
	AVCodecContext* 	pCodecCtx;  
	AVCodec* 			pCodec;  
	AVFrame*			pFrame;
	AVFrame* 			pFrameYUV;
	SimpliedFFmpegMeta()
	{
		pFormatCtx = NULL;
		pCodecCtx = NULL;
		pCodec = NULL;
		pFrame  = NULL;
		pFrameYUV = NULL;
	}
};

int CSimpliedStream::init()
{
	av_register_all();  
	avformat_network_init();
	return 0;
}
int CSimpliedStream::unInit()
{
	return 0;
}

int	CSimpliedStream::startRecvStreamProc(CXXSimpliedStreamMsgProc* pMsgProc)
{

	return 0;
}
int	CSimpliedStream::recvStreamProc(void* pMsgProc)
{
	if(pMsgProc == NULL)
	{
		return -1;
	}
	CXXSimpliedStreamMsgProc* pStreamProc = (CXXSimpliedStreamMsgProc*)pMsgProc;
	AVPacket* pPacket=(AVPacket *)av_malloc(sizeof(AVPacket));  
	while(1)
	{
		if(pStreamProc->meta.nQuit)
		{
			break;
		}
		
		if(av_read_frame((AVFormatContext*)(pStreamProc->meta.pFormatCtx),pPacket) >=0)  
		{  
			if(pPacket->stream_index== pStreamProc->meta.streamIndex)  
			{  
				if(pStreamProc->pFunc != NULL)
				{
					 pStreamProc->pFunc(NULL,0,pPacket->data,pPacket->size,pStreamProc->pUserData1,pStreamProc->pUserData2);
				}
			}  
			av_free_packet(pPacket);  
		}  

	}
	return 0;
}
int CSimpliedStream::openStream(const char* pszServer,PSimpliedStreamFunc pFunc,void* pUserData1,void* pUserData2)
{
	
	CXXSimpliedStreamMsgProc streamMsgProc;
	
	AVFormatContext *pFormatCtx;  
	int             i, videoindex;  
	AVCodecContext  *pCodecCtx;  
	AVCodec         *pCodec;  
	AVFrame *pFrame,*pFrameYUV;  
	uint8_t *out_buffer;  
	AVPacket *packet;  
	int ret, got_picture;  
  
	struct SwsContext *img_convert_ctx;  
  
  
	pFormatCtx = avformat_alloc_context();  
  
	if(avformat_open_input(&pFormatCtx,pszServer,NULL,NULL)!=0)////打开网络流或文件流  
	{  
		printf("Couldn't open input stream.\n");  
		return -1;  
	}  
	if(avformat_find_stream_info(pFormatCtx,NULL)<0)  
	{  
		printf("Couldn't find stream information.\n");  
		return -1;  
	}  
	videoindex=-1;  
	for(i=0; i<pFormatCtx->nb_streams; i++)   
	if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)  
	{  
		videoindex=i;  
		break;  
	}  
	if(videoindex==-1)  
	{  
		printf("Didn't find a video stream.\n");  
		return -1;  
	}  
	pCodecCtx=pFormatCtx->streams[videoindex]->codec;  
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);  
	if(pCodec==NULL)  
	{  
		printf("Codec not found.\n");  
		return -1;  
	}  
	if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)  
	{  
		printf("Could not open codec.\n");  
		return -1;  
	}  
	pFrame=av_frame_alloc();  
	pFrameYUV=av_frame_alloc();  
	out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));  
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);  

	//Output Info---输出一些文件（RTSP）信息  
	printf("---------------- File Information ---------------\n");  
	av_dump_format(pFormatCtx,0,pszServer,0);  
	printf("-------------------------------------------------\n");  

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,   
		pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);   

	streamMsgProc.meta.pFormatCtx = pFormatCtx;
	streamMsgProc.meta.pCodecCtx = pCodecCtx;
	streamMsgProc.meta.pCodec = pCodec;
	streamMsgProc.meta.pFrame = pFrame;
	streamMsgProc.meta.pFrameYUV = pFrameYUV;
	streamMsgProc.meta.streamIndex = videoindex;
	streamMsgProc.pFunc = pFunc;
	streamMsgProc.pUserData1 = pUserData1;
	streamMsgProc.pUserData2 = pUserData2;
	
	addStreamMsgProc(pszServer,&streamMsgProc);

	CXXSimpliedStreamMsgProc* pMsgProc = getStreamMsgProc(pszServer);
	if(pMsgProc != NULL)
	{
		SDL_Thread * sdl_work_tid = SDL_CreateThread(CSimpliedStream::recvStreamProc,NULL,(void*)pMsgProc);
		pMsgProc->meta.pThread = sdl_work_tid;
	}
					
	/*
	packet=(AVPacket *)av_malloc(sizeof(AVPacket));  


	FILE *fpSave;  
	if((fpSave = fopen("geth264.h264", "ab")) == NULL) //h264保存的文件名  
		return 0;   
	for (;;)   
	{  
		//------------------------------  
		if(av_read_frame(pFormatCtx, packet)>=0)  
		{  
			if(packet->stream_index==videoindex)  
			{  
				fwrite(packet->data,1,packet->size,fpSave);//写数据到文件中  
			}  
			av_free_packet(packet);  
		}  
	} */
	return 0;  
}

int CSimpliedStream::closeStream(const char* pszServer)
{
	CXXSimpliedStreamMsgProc* pStreamProc = getStreamMsgProc(pszServer);
	if(pStreamProc != NULL)
	{
		pStreamProc->meta.nQuit  = 1;
	}
	removeStreamMsgProc(pszServer);
	return 0;
}
//-------------- 
int CSimpliedStream::removeStreamMsgProc(CXXSimpliedStreamMsgProc* pMsgProc) 
{
	if(pMsgProc == NULL)
	{
		return -1;
	}
	CXXSimpliedFFmpegMeta& meta = pMsgProc->meta;

	if(meta.pThread != NULL)
	{
		int nWaitStatus  = 0;
		//SDL_DetachThread((SDL_Thread*)meta.pThread);
		SDL_WaitThread((SDL_Thread*)meta.pThread,&nWaitStatus);
		meta.pThread = NULL;
	}
	if(meta.pFrameYUV != NULL)
	{
		av_frame_free((AVFrame**)(&meta.pFrameYUV));
		meta.pFrameYUV = NULL;
	}
	if(meta.pFrame != NULL)
	{
		av_frame_free((AVFrame**)(&meta.pFrame));
		meta.pFrame  = NULL;
	}
	if(meta.pCodecCtx  != NULL)
	{
		avcodec_close((AVCodecContext*)(meta.pCodecCtx));
		meta.pCodecCtx  = NULL;
	}
	if(meta.pFormatCtx != NULL)
	{
		avformat_close_input((AVFormatContext**)(&meta.pFormatCtx)); 
		meta.pFormatCtx = NULL;
	}
	return 0;
}

int CSimpliedStream::addStreamMsgProc(const char* pszFlag,CXXSimpliedStreamMsgProc* pMsgProc)
{
	if(pszFlag == NULL || pMsgProc == NULL)
	{
		return -1;
	}
	m_streamMsgProcMap[pszFlag] = *pMsgProc;
	return 0;
}

int CSimpliedStream::removeStreamMsgProc(const char* pszFlag)
{
	if(pszFlag == NULL)
	{
		return -1;
	}
	std::map<std::string,CXXSimpliedStreamMsgProc>& refMap = m_streamMsgProcMap;
	std::map<std::string,CXXSimpliedStreamMsgProc>::iterator mapIter = refMap.find(pszFlag);
	if(mapIter != refMap.end())
	{
		removeStreamMsgProc(&mapIter->second);
		refMap.erase(pszFlag);
	}
	return 0;
}

CXXSimpliedStreamMsgProc* CSimpliedStream::getStreamMsgProc(const char* pszFlag)
{
	if(pszFlag == NULL)
	{
		return NULL;
	}
	CXXSimpliedStreamMsgProc* pProc = NULL;
	std::map<std::string,CXXSimpliedStreamMsgProc>& refMap = m_streamMsgProcMap;
	std::map<std::string,CXXSimpliedStreamMsgProc>::iterator mapIter = refMap.find(pszFlag);
	if(mapIter != refMap.end())
	{
		pProc = &mapIter->second;
		
	}
	return pProc;
}
CSimpliedStream::~CSimpliedStream()
{
	CXXSimpliedStreamMsgProc* pProc = NULL;
	std::map<std::string,CXXSimpliedStreamMsgProc>& refMap = m_streamMsgProcMap;
	std::map<std::string,CXXSimpliedStreamMsgProc>::iterator mapIter = refMap.begin();
	for(;mapIter != refMap.end();mapIter++)
	{
		pProc = &mapIter->second;
		pProc->meta.nQuit = 1;
		removeStreamMsgProc(pProc);
	}
	refMap.clear();
}
/*
int main(int argc, char* argv[])  
{  
  
	AVFormatContext *pFormatCtx;  
	int             i, videoindex;  
	AVCodecContext  *pCodecCtx;  
	AVCodec         *pCodec;  
	AVFrame *pFrame,*pFrameYUV;  
	uint8_t *out_buffer;  
	AVPacket *packet;  
	int ret, got_picture;  
  
  
	struct SwsContext *img_convert_ctx;  
	//下面是公共的RTSP测试地址  
	char filepath[]="rtsp://218.204.223.237:554/live/1/0547424F573B085C/gsfp90ef4k0a6iap.sdp";  
  
	av_register_all();  
	avformat_network_init();  
	pFormatCtx = avformat_alloc_context();  
  
	if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0)////打开网络流或文件流  
	{  
		printf("Couldn't open input stream.\n");  
		return -1;  
	}  
	if(avformat_find_stream_info(pFormatCtx,NULL)<0)  
	{  
		printf("Couldn't find stream information.\n");  
		return -1;  
	}  
	videoindex=-1;  
	for(i=0; i<pFormatCtx->nb_streams; i++)   
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)  
		{  
			videoindex=i;  
			break;  
		}  
		if(videoindex==-1)  
		{  
			printf("Didn't find a video stream.\n");  
			return -1;  
		}  
		pCodecCtx=pFormatCtx->streams[videoindex]->codec;  
		pCodec=avcodec_find_decoder(pCodecCtx->codec_id);  
		if(pCodec==NULL)  
		{  
			printf("Codec not found.\n");  
			return -1;  
		}  
		if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)  
		{  
			printf("Could not open codec.\n");  
			return -1;  
		}  
		pFrame=av_frame_alloc();  
		pFrameYUV=av_frame_alloc();  
		out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));  
		avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);  
  
		//Output Info---输出一些文件（RTSP）信息  
		printf("---------------- File Information ---------------\n");  
		av_dump_format(pFormatCtx,0,filepath,0);  
		printf("-------------------------------------------------\n");  
  
		img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,   
			pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);   
  
  
		packet=(AVPacket *)av_malloc(sizeof(AVPacket));  
  
		FILE *fpSave;  
		if((fpSave = fopen("geth264.h264", "ab")) == NULL) //h264保存的文件名  
			return 0;   
		for (;;)   
		{  
			//------------------------------  
			if(av_read_frame(pFormatCtx, packet)>=0)  
			{  
				if(packet->stream_index==videoindex)  
				{  
					fwrite(packet->data,1,packet->size,fpSave);//写数据到文件中  
				}  
				av_free_packet(packet);  
			}  
		}  
  
  
		//--------------  
		av_frame_free(&pFrameYUV);  
		av_frame_free(&pFrame);  
		avcodec_close(pCodecCtx);  
		avformat_close_input(&pFormatCtx);  
  
		return 0;  
} 
*/ 