// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include<boost/asio.hpp>
#include<boost/bind.hpp>
#include<boost/array.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<string>
#include<fstream>
#include<sstream>
#include<cstring>

//#define __DEBUG__

#ifdef __DEBUG__
#include <iostream>
#endif

#include "communication_protocoll.hh"

using boost::asio::ip::tcp;

struct ClientSessionParam{
	std::string message;
	std::string ccppl_file_path;
};

class ClientSession:public boost::enable_shared_from_this<ClientSession>{
	public:
		typedef boost::shared_ptr<ClientSession> pointer;
		typedef ClientSessionParam parameter;

		static pointer create(boost::asio::io_service & io_service, parameter & param){
			return pointer(new ClientSession(io_service, param));
		}

		tcp::socket & socket(){
			return _socket;
		}

		void start(){async_write();}

	private:
		ClientSession(boost::asio::io_service &io_service, parameter & param)
			:_socket(io_service),
			_file(param.ccppl_file_path, std::ios_base::binary | std::ios_base::ate),
			_message(std::move(param.message)),
			_buf(static_cast<size_t>(_file.tellg())
					+ _message.size()
					+ static_cast<size_t>(PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN))
		{
			prepare_message();
		}

		void prepare_message(){
			size_t messageLen = _message.size();
			size_t ccpplLen = _file.tellg();

			*reinterpret_cast<package_type_t *>(&_buf[0]) = PackageType::POLICY_ENABLED_DATA;
			*reinterpret_cast<data_len_t *>(&_buf[0] + PackageHeaderFieldLen::TYPE) = messageLen;
			*reinterpret_cast<policy_len_t *>(&_buf[0] + PackageHeaderFieldLen::COMMON_HEADER_LEN) = ccpplLen;

			//dauto pMessage = _buf.begin() + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN;
			char * pMessage = &_buf[0] + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN;
			char * pCcppl = (&_buf[0] + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN + messageLen);

			//_buf.insert(pMessage, _message.begin(), _message.end());
			memcpy(pMessage, _message.c_str(), messageLen);
			_file.seekg(0);
			_file.read(pCcppl, ccpplLen);
		}

		void async_write(){
			boost::asio::async_write(_socket, boost::asio::buffer(_buf,_buf.size()),
					boost::asio::transfer_all(),
					boost::bind(&ClientSession::handle_write, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		void handle_write(const boost::system::error_code & error, size_t bytes_transferred){
#ifdef __DEBUG__
			if (!error)
				std::cout<<"bytes transfered: "<<bytes_transferred<<std::endl;
#endif
		}

		tcp::socket _socket;
		std::ifstream _file;
		std::string _message;
		std::vector<char> _buf;
};
