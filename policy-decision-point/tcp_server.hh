#pragma once

#include<boost/asio.hpp>
#include<boost/bind.hpp>

using boost::asio::ip::tcp;

template<typename Session>
class TCPServer{
	public:
		TCPServer(boost::asio::io_service & io_service, unsigned short port)
			:acceptor(io_service,tcp::endpoint(tcp::v4(),port)){
				start_accept();
			}

	private:
		void start_accept(){
			typename Session::pointer new_session = Session::create(acceptor.get_io_service());

			acceptor.async_accept(new_session->socket(),
					boost::bind(&TCPServer::handle_accept,this, new_session,
						boost::asio::placeholders::error));
		}

		void handle_accept(typename Session::pointer new_session, const boost::system::error_code error){
			if (!error){
				new_session->start();
			}

			start_accept();
		}

		tcp::acceptor acceptor;
};
