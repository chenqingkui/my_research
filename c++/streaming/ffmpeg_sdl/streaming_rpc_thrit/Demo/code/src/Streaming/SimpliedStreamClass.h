#ifndef _SIMPLIED_STREAM_CLASS_H_
#define _SIMPLIED_STREAM_CLASS_H_
#include <string>
#include <map>
typedef int (*PSimpliedStreamFunc)(void* pHandle,int nType,void* pStream,int nStreamLen,void* pUserData1,void* pUserData2);
struct CXXSimpliedFFmpegMeta
{
	void* 			pFormatCtx;   
	void* 			pCodecCtx;  
	void* 			pCodec;  
	void*			pFrame;
	void* 			pFrameYUV;
	int				streamIndex;
	void*			pThread;
	int				nQuit;
	CXXSimpliedFFmpegMeta()
	{
		pFormatCtx = NULL;
		pCodecCtx = NULL;
		pCodec = NULL;
		pFrame  = NULL;
		pFrameYUV = NULL;
		streamIndex = 0;
		pThread = NULL;
		nQuit = 0;
	}
};

struct CXXSimpliedStreamMsgProc
{
	CXXSimpliedFFmpegMeta meta;
	PSimpliedStreamFunc pFunc;
	void*			   pUserData1;
	void*			   pUserData2;
	CXXSimpliedStreamMsgProc()
	{
		pFunc = NULL;
		pUserData1 = NULL;
		pUserData2 = NULL;
	}
};
class CSimpliedStream
{  
private:
	static int	startRecvStreamProc(CXXSimpliedStreamMsgProc* pMsgProc);
	std::map<std::string,CXXSimpliedStreamMsgProc>   m_streamMsgProcMap;
public:
	static int recvStreamProc(void* pMsg);
public:
	~CSimpliedStream();
public:
	int init();
	int unInit();
	int openStream(const char* pszServer,PSimpliedStreamFunc pFunc,void* pUserData1,void* pUserData2);
	int closeStream(const char* pszServer);
	//int setStreamHandle(PSimpliedStreamFunc pFunc,void* pUserData1,void* pUserData2);
	//int getStreamHandle(PSimpliedStreamFunc& pFunc,void* & pUserData1,void* & pUserData2);
	int removeStreamMsgProc(CXXSimpliedStreamMsgProc* pMsgProc); 
	int removeStreamMsgProc(const char* pszFlag); 
	int addStreamMsgProc(const char* pszFlag,CXXSimpliedStreamMsgProc* pMsgProc);
	CXXSimpliedStreamMsgProc* getStreamMsgProc(const char* pszFlag);
};
#endif