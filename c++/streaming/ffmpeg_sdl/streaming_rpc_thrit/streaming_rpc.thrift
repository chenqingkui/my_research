namespace cpp com.lesgo.thrift.rpc.streaming

/**
* 协议类型 
* PROTOCOL_TYPE_RTSP rtsp
* PROTOCOL_TYPE_SIP  sip
* PROTOCOL_TYPE_ONVIF onvif
*/
enum PROTOCOL_TYPE
{
	PROTOCOL_TYPE_RTSP 		= 11,
	PROTOCOL_TYPE_SIP	  	= 12,
	PROTOCOL_TYPE_ONVIF	  	= 13,
}

/**
* 协议类型 
* STREAMING_TYPE_ES  es
* STREAMING_TYPE_PS  ps
* STREAMING_TYPE_TS  ts
*/

enum STREAMING_TYPE
{
	STREAMING_TYPE_ES		= 101,
	STREAMING_TYPE_PS		= 102,
	STREAMING_TYPE_TS		= 103,
}

/**
* 请求推送Sip协议
* msgId  消息ID
* streamingType 请求视频流类型
* pushServerUrl 推送服务器URL
* streamID      视频流ID
*/
struct PushSipStream
{
	1: required string 					msgId;
	2: required string					streamingType;
	3: required string					serverUrl;
	4: required string 					streamID;
}

/**
* 请求推送rtsp协议
* msgId  消息ID
* streamingType 请求视频流类型
* pushServerUrl 推送服务器URL
* streamID      视频流ID
* streamURL      视频流URL
*/
struct PushRtspStream
{
	1: required string 					msgId;
	2: required string					streamingType;
	3: required string					serverUrl;
	4: required string 					streamID;
	5: required string 					streamURL;
}

/**
* 请求推送Onvif协议
* msgId  消息ID
* streamingType 请求视频流类型
* pushServerUrl 推送服务器URL
* streamID      视频流ID
* streamURL      视频流URL
*/
struct PushOnvifStream
{
	1: required string 					msgId;
	2: required string					streamingType;
	3: required string					serverUrl;
	4: required string 					streamID;
	5: required string 					streamURL;
}



/**
* 停止推送Sip协议
* msgId  消息ID
* streamingType 请求视频流类型
* pushServerUrl 推送服务器URL
* streamID      视频流ID
*/
struct StopSipStream
{
	1: required string 					msgId;
	2: required string 					streamID;
	3: optional string                  serverUrl;
}
/**
* 停止推送rtsp协议
* msgId  消息ID
* streamingType 请求视频流类型
* pushServerUrl 推送服务器URL
* streamID      视频流ID
* streamURL      视频流URL
*/
struct StopRtspStream
{
	1: required string 					msgId;
	2: required string 					streamID;
	3: required string 					streamURL;
	4: optional string                  serverUrl;
}
/**
* 停止推送Onvif协议
* msgId  消息ID
* streamingType 请求视频流类型
* pushServerUrl 推送服务器URL
* streamID      视频流ID
* streamURL      视频流URL
*/
struct StopOnvifStream
{
	1: required string 					msgId;
	2: required string 					streamID;
	3: required string 					streamURL;
	4: optional string                  serverUrl;
}

/**
* 请求消息服务
* pushSipStreaming  请求推送sip协议视频流
* pushRtspStream    请求推送rtsp协议视频流
* pushOnvifStream   请求推送Onvif协议流数据
*/
service StreamingRPCService
{
	i32 	pushSipStreaming(1:PushSipStream reqStream);
	i32 	pushRtspStreaming(1:PushRtspStream reqStream);
	i32 	pushOnvifStreaming(1:PushOnvifStream reqStream);
	i32 	stopSipStreaming(1:StopSipStream reqStream);
	i32 	stopRtspStreaming(1:StopRtspStream reqStream);
	i32 	stopOnvifStreaming(1:StopOnvifStream reqStream);
}