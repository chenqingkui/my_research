// This autogenerated skeleton file illustrates how to build a server.
// You should copy it to another filename to avoid overwriting it.

#include "StreamingRPCService.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "StreamExport.h"
using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

using namespace  ::com::lesgo::thrift::rpc::streaming;

class StreamingRPCServiceHandler : virtual public StreamingRPCServiceIf {
private:
	CStreamExport*	pStream; 
public:
  StreamingRPCServiceHandler() {
    // Your initialization goes here
	pStream = new CStreamExport;
  }

  int32_t pushSipStreaming(const PushSipStream& reqStream) {
    // Your implementation goes here
    printf("pushSipStreaming\n");
	if(pStream != NULL)
	{
		pStream->startStream(reqStream.streamID.c_str(),"",reqStream.serverUrl.c_str());

	}
	return 0;
  }

  int32_t pushRtspStreaming(const PushRtspStream& reqStream) {
    // Your implementation goes here
    printf("pushRtspStreaming\n");
	if(pStream != NULL)
	{
		pStream->startStream(reqStream.streamID.c_str(),reqStream.streamURL.c_str(),reqStream.serverUrl.c_str());

	}
	return 0;
  }

  int32_t pushOnvifStreaming(const PushOnvifStream& reqStream) {
    // Your implementation goes here
    printf("pushOnvifStreaming\n");
	if(pStream != NULL)
	{
		pStream->startStream(reqStream.streamID.c_str(),reqStream.streamURL.c_str(),reqStream.serverUrl.c_str());

	}
	return 0;
  }

  int32_t stopSipStreaming(const StopSipStream& reqStream) {
    // Your implementation goes here
    printf("stopSipStreaming\n");
	if(pStream != NULL)
	{
		pStream->stopStream(reqStream.streamID.c_str(),"");
	}
	return 0;
  }

  int32_t stopRtspStreaming(const StopRtspStream& reqStream) {
    // Your implementation goes here
    printf("stopRtspStreaming\n");
	if(pStream != NULL)
	{
		pStream->stopStream(reqStream.streamID.c_str(),"");
	}
	return 0;
  }

  int32_t stopOnvifStreaming(const StopOnvifStream& reqStream) {
    // Your implementation goes here
    printf("stopOnvifStreaming\n");
	if(pStream != NULL)
	{
		pStream->stopStream(reqStream.streamID.c_str(),reqStream.streamURL.c_str());
	}
	return 0;
  }

};

int main(int argc, char **argv) {
  int port = 9090;
  shared_ptr<StreamingRPCServiceHandler> handler(new StreamingRPCServiceHandler());
  shared_ptr<TProcessor> processor(new StreamingRPCServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

  TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
  server.serve();

  CStreamExport* pStream = new CStreamExport;
  if(pStream != NULL)
  {
		
		//std::string pushUrl = "ws://34.80.253.16:19093/multiprocessStream/websocket/v1/stream/recv/connects/b5939dc9bbeb04aa44c1c35a62a47abf";
		std::string pushUrl = "ws://127.0.0.1:9002";
		pStream->startStream("1234","rtsp://34.80.253.16:5454/test1-rtsp.avi",pushUrl.c_str());
		//pStream->startStream("1234","rtsp://192.168.9.227:5454/test1-rtsp.avi",pushUrl.c_str()); 
  }
	std::list<std::string>* pList = new std::list<std::string>;
	void* pList2 = new std::list<std::string>;
	for(int i = 0;i < 1000000;i++)
	{
		Sleep(100);
	}
	
	system("pause");
	
	return 0;
  return 0;
}

