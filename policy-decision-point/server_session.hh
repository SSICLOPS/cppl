#pragma once

#include<boost/asio.hpp>
#include<boost/bind.hpp>
#include<boost/array.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<string>
#include<fstream>
#include<sstream>

//#define __DEBUG__

#ifdef __DEBUG__
#include <iostream>
#endif

#include "communication_protocoll.hh"

using boost::asio::ip::tcp;

struct ServerSessionParam{
	std::string node_param_file_path;
	std::string runtim_param_file_path;
};

class ServerSession:public boost::enable_shared_from_this<ServerSession>{
	public:
		typedef boost::shared_ptr<ServerSession> pointer;
		typedef ServerSessionParam parameter;

		static pointer create(boost::asio::io_service & io_service, parameter & param){
			return pointer(new ServerSession(io_service, param));
		}

		tcp::socket & socket(){
			return _socket;
		}

		void start(){async_write();}

	private:
		ServerSession(boost::asio::io_service &io_service, parameter & file_path)
			:_socket(io_service),
			_file_node_param(file_path.node_param_file_path, std::ios_base::binary | std::ios_base::ate),
			_file_runtime_param(file_path.runtim_param_file_path, std::ios_base::binary | std::ios_base::ate),
			_buf(static_cast<size_t>(_file_node_param.tellg())
					+ static_cast<size_t>(_file_runtime_param.tellg())
					+ static_cast<size_t>(PackageHeaderFieldLen::COMMON_HEADER_LEN)
					+ 3)
		{
			prepare_message();
		}

		void prepare_message(){
			*reinterpret_cast<package_type_t *>(&_buf[0]) = PackageType::NODE_PARAM;
			*reinterpret_cast<data_len_t *>(&_buf[0] + PackageHeaderFieldLen::TYPE) = _buf.size() - PackageHeaderFieldLen::COMMON_HEADER_LEN;

			char * pNodeParamContent = &_buf[0] + PackageHeaderFieldLen::COMMON_HEADER_LEN;
			size_t NodeParamContenLen = _file_node_param.tellg();
			char * pRuntimeParamContent = pNodeParamContent + NodeParamContenLen + 1;
			size_t RuntimeParamContentLen = _file_runtime_param.tellg();

			*pNodeParamContent = '[';
			_file_node_param.seekg(0);
			_file_node_param.read(pNodeParamContent + 1, NodeParamContenLen);

			*pRuntimeParamContent = ',';
			_file_runtime_param.seekg(0);
			_file_runtime_param.read(pRuntimeParamContent + 1, RuntimeParamContentLen);

			_buf[_buf.size() - 1] = ']';

			_file_node_param.close();
			_file_runtime_param.close();
		}

		void async_write(){
			boost::asio::async_write(_socket, boost::asio::buffer(_buf,_buf.size()),
					boost::asio::transfer_all(),
					boost::bind(&ServerSession::handle_write, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		void handle_write(const boost::system::error_code & error, size_t bytes_transferred){
		}

		tcp::socket _socket;
		std::ifstream _file_node_param;
		std::ifstream _file_runtime_param;
		std::vector<char> _buf;
};
