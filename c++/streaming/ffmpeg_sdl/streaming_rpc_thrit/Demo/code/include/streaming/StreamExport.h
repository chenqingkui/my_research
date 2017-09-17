#ifndef _STREAM_EXPORT_H_
#define _STREAM_EXPORT_H_
#ifdef WIN32  
	#ifdef __cplusplus  
		#define DLL_EXPORT_C_DECL extern "C" __declspec(dllexport)  
		#define DLL_IMPORT_C_DECL extern "C" __declspec(dllimport)  
		#define DLL_EXPORT_DECL extern __declspec(dllexport)  
		#define DLL_IMPORT_DECL extern __declspec(dllimport)  
		#define DLL_EXPORT_CLASS_DECL __declspec(dllexport)  
		#define DLL_IMPORT_CLASS_DECL __declspec(dllimport)  
	#else  
		#define DLL_EXPORT_DECL __declspec(dllexport)  
		#define DLL_IMPORT_DECL __declspec(dllimport)  
	#endif  
#else  
	#ifdef __cplusplus  
		#define DLL_EXPORT_C_DECL extern "C"  
		#define DLL_IMPORT_C_DECL extern "C"  
		#define DLL_EXPORT_DECL extern  
		#define DLL_IMPORT_DECL extern  
		#define DLL_EXPORT_CLASS_DECL  
		#define DLL_IMPORT_CLASS_DECL  
	#else  
		#define DLL_EXPORT_DECL extern  
		#define DLL_IMPORT_DECL extern  
	#endif  
#endif  
#ifdef STREAM_EXPORTS  
	#define STREAM_CLASS DLL_EXPORT_CLASS_DECL  
	#define STREAM_API DLL_EXPORT_DECL  
#else  
	#define STREAM_CLASS DLL_IMPORT_CLASS_DECL  
	#define STREAM_API DLL_IMPORT_DECL  
#endif  

class STREAM_CLASS CStreamExport
{
  private:
    void* m_pStream;
	void* m_pStreamList;
	void* m_hLock;
	void* m_pSendThread;
	void* m_pStreamCon;
	void* m_pStreamConMeta;
  public:
    CStreamExport();
  private:
		int createLock();
		int destroyLock();
  private:
		int lock();
		int unlock();
  private:
	    void* createThread();
  private:
    int sendStream(const char* pDstUrl,void* pData,int nDataSize);
  public:
	void* createStreamConnect(const char* pDstUrl);
	int	  stopStreamConnect(const char* pszUrl);
	int sendStream(void* pStream,void* pID,void* pData,int nDataSize);
	int  connectWSS(void*& pWSCli,const char* pszUrl,void*& pWSId);
	int getStreamHandle(void*& pWSCli,void*& pID);
	int	setStreamHandle(void* pWSCli,void* pID);
  public:
    int startStream(const char* pStreamId,const char* psrcUrl,const char* pDstUrl);
    int stopStream(const char* pStreamId,const char* psrcUrl);
	int recvStream(int nType,void* pData,int nDataSize);
	int popStreamMeta(void*& pBuf,int& nBufLen, int& nType);
};
#endif