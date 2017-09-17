#include "WebSocketClient.hpp"
//#include <websocketpp/config/asio_no_tls_client.hpp>
//#include <websocketpp/client.hpp>

#include <websocketpp/common/thread.hpp>
//#include <websocketpp/common/memory.hpp>

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

//#include <boost/function.hpp>
//#include <boost/bind.hpp>
namespace WEB
{
	namespace CLIENT
	{
		
		
		CWebSocketClient::CWebSocketClient()
		{
			
			m_pClient = NULL;
			//m_pThread = NULL;
			//m_pConnect = NULL;
			
			websocketpp_aiso_client* pClient = NULL;
			pClient = new websocketpp_aiso_client;
			m_pClient = pClient;
			if(m_pClient == NULL)
			{
				return ;
			}
			pClient->clear_access_channels(websocketpp::log::alevel::all);
			pClient->clear_error_channels(websocketpp::log::elevel::all);

			pClient->init_asio();
			pClient->start_perpetual();
			//m_pThread = (void*)&websocketpp::lib::make_shared<websocketpp::lib::thread>(&websocketpp_aiso_client::run, m_pClient);
			m_pThread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&websocketpp_aiso_client::run, pClient);
			
		}
		CWebSocketClient::~CWebSocketClient()
		{
		
			websocketpp_aiso_client* pClient = (websocketpp_aiso_client*)m_pClient;
			if(pClient == NULL)
			{
				return ;
			}
			pClient->stop_perpetual();
			
			websocketpp_aiso_client::connection_ptr pConnect = (websocketpp_aiso_client::connection_ptr)m_pConnect;

			websocketpp::lib::error_code ec;
			
			
			if(pConnect != NULL)
			{
				pClient->close(pConnect->get_handle(), websocketpp::close::status::going_away, "", ec);
				pConnect = NULL;
				m_pConnect = NULL;
			}
			if (ec)
			{

			}
			
			websocketpp::lib::shared_ptr<websocketpp::lib::thread> pThread = (websocketpp::lib::shared_ptr<websocketpp::lib::thread>)m_pThread;
			if(pThread != NULL)
			{
				pThread->join();
			}
			
			if(pClient != NULL)
			{
				delete pClient;
				pClient = NULL;
				m_pClient = NULL;
			}	
		}
		void CWebSocketClient::on_open(websocketpp_aiso_client * c, websocketpp::connection_hdl hdl) 
		{
			
			m_status = "Open";
			websocketpp_aiso_client::connection_ptr con = c->get_con_from_hdl(hdl);
			con->get_response_header("Server");

			std::cout<<__FUNCTION__<<std::endl;
			
		}

		void CWebSocketClient::on_fail(websocketpp_aiso_client * c, websocketpp::connection_hdl hdl) 
		{
			
			m_status = "Failed";
			websocketpp_aiso_client::connection_ptr con = c->get_con_from_hdl(hdl);
			std::cout<<__FUNCTION__<<std::endl;
			
		}

		void CWebSocketClient::on_close(websocketpp_aiso_client * c, websocketpp::connection_hdl hdl) 
		{
			
			websocketpp_aiso_client::connection_ptr con = c->get_con_from_hdl(hdl);
			std::stringstream s;
			s << "close code: " << con->get_remote_close_code() << " (" 
				<< websocketpp::close::status::get_string(con->get_remote_close_code()) 
				<< "), close reason: " << con->get_remote_close_reason();
			std::cout<<__FUNCTION__<<std::endl;
			
		}

		void CWebSocketClient::on_message(websocketpp::connection_hdl, websocketpp_aiso_client::message_ptr msg) 
		{
			if (msg->get_opcode() == websocketpp::frame::opcode::text) 
			{
				
			}	 
			else 
			{
			}
		}
		int	CWebSocketClient::sendText(const char* pszText)
		{
			websocketpp_aiso_client* pClient = (websocketpp_aiso_client*)m_pClient;
			if(pClient == NULL)
			{
				return -1 ;
			}
			websocketpp::lib::error_code ec;
			if(m_pConnect != NULL)
			{
				pClient->send(m_pConnect, pszText, websocketpp::frame::opcode::text, ec);
				if (ec)
				{
					std::cout << "> Error sending message: " << ec.message() << std::endl;
					return -1;
				}
			}
			return 0;
		}
		int CWebSocketClient::sendBinary(void* pData,int nDataLen)
		{	
			websocketpp_aiso_client* pClient = (websocketpp_aiso_client*)m_pClient;
			if(pClient == NULL)
			{
				return -1;
			}
			websocketpp::lib::error_code ec;
			if(m_pConnect != NULL)
			{
				pClient->send(m_pConnect, pData,nDataLen, websocketpp::frame::opcode::binary, ec);
				if (ec)
				{
					std::cout << "> Error sending message: " << ec.message() << std::endl;
					return -1;
				}
			}
			return 0;
		}
		int CWebSocketClient::connect(const char* pszUrl)
		{
			
			websocketpp_aiso_client* pClient =  (websocketpp_aiso_client*)m_pClient;
			if(pClient == NULL)
			{
				return -1;
			}
			websocketpp::lib::error_code ec;

			websocketpp_aiso_client::connection_ptr pConnect = pClient->get_connection(pszUrl, ec);

			if (ec) {
				std::cout << "> Connect initialization error: " << ec.message() << std::endl;
				return -1;
			}				
			
			//websocketpp::lib::shared_ptr<CWebSocketClient> meta_ptr = websocketpp::lib::make_shared<CWebSocketClient>();
			
			
			pConnect->set_open_handler(websocketpp::lib::bind(
						&CWebSocketClient::on_open,
						//meta_ptr,
						this,
						pClient,
						websocketpp::lib::placeholders::_1
						));
			pConnect->set_fail_handler(websocketpp::lib::bind(
						&CWebSocketClient::on_fail,
						//meta_ptr,
						this,
						pClient,
						websocketpp::lib::placeholders::_1
						));
			pConnect->set_close_handler(websocketpp::lib::bind(
						&CWebSocketClient::on_close,
						//meta_ptr,
						this,
						pClient,
						websocketpp::lib::placeholders::_1
						));
			pConnect->set_message_handler(websocketpp::lib::bind(
						&CWebSocketClient::on_message,
						//meta_ptr,
						this,
						websocketpp::lib::placeholders::_1,
						websocketpp::lib::placeholders::_2
						));

			
			m_pConnect = pConnect;
			
			pClient->connect(pConnect);
			return 0;
		}
		int CWebSocketClient::disconnect()
		{
			websocketpp_aiso_client* pClient = (websocketpp_aiso_client*)m_pClient;
			if(pClient == NULL)
			{
				return -1;
			}
			websocketpp_aiso_client::connection_ptr pConnect = (websocketpp_aiso_client::connection_ptr)m_pConnect;

			websocketpp::lib::error_code ec;

			if(pConnect != NULL)
			{
				pClient->close(pConnect->get_handle(), websocketpp::close::status::going_away, "", ec);
				pConnect = NULL;
				m_pConnect = NULL;
			}
			if (ec)
			{

			}
			return 0;		
		}
	}
}