#ifndef _WEB_SOCKET_CLIENT_H_
#define _WEB_SOCKET_CLIENT_H_
#include <string>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
namespace WEB
{
	namespace CLIENT
	{
		
		typedef websocketpp::client<websocketpp::config::asio_client> websocketpp_aiso_client;
		typedef websocketpp::lib::shared_ptr<websocketpp::lib::thread>   websocketpp_shared_ptr_thread;
		typedef websocketpp_aiso_client::connection_ptr					 websocketpp_asio_client_conn_ptr;
		
		class CWebSocketClient
		{
			private:
				void*	m_pClient;
				//void* 	m_pThread;
				//void* 	m_pConnect;
				websocketpp_shared_ptr_thread		m_pThread;
				websocketpp_asio_client_conn_ptr	m_pConnect;
			private:
				std::string m_status;
			public:
				CWebSocketClient();
				~CWebSocketClient();
			public:
				
				void on_open(websocketpp_aiso_client * c, websocketpp::connection_hdl hdl);
				void on_fail(websocketpp_aiso_client * c, websocketpp::connection_hdl hdl);
				void on_close(websocketpp_aiso_client * c, websocketpp::connection_hdl hdl);
				void on_message(websocketpp::connection_hdl, websocketpp_aiso_client::message_ptr msg);
			
			public:
				int	sendText(const char* pszText);
				int sendBinary(void* pData,int nDataLen);
				int connect(const char* pszUrl);
				int disconnect();
		};
	}
}
#endif
