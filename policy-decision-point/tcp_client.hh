#pragma once

#include<boost/asio.hpp>
#include<boost/bind.hpp>
#include<string>

using boost::asio::ip::tcp;

template<typename Session>
class TCPClient{
	public:
		TCPClient(boost::asio::io_service & io_service, const std::string ip, const std::string port, typename Session::parameter & param)
			:resolver(io_service)
		{
			tcp::resolver::query query(ip, port);
			_endpoint_iterator = resolver.resolve(query);

			start_connection(param);
		}
	private:
		void start_connection(typename Session::parameter & param){
			typename Session::pointer new_session = Session::create(resolver.get_io_service(), param);

			boost::asio::async_connect(new_session->socket(),_endpoint_iterator,
					boost::bind(&TCPClient::handle_connection, this, new_session,
						boost::asio::placeholders::error));
		}
		void handle_connection(typename Session::pointer new_session, const boost::system::error_code error){
			if (!error){
				new_session->start();
			}
		}

		tcp::resolver resolver;
		tcp::resolver::iterator _endpoint_iterator;
};
