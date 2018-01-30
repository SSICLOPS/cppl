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

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

//#define __DEBUG__

#ifdef __DEBUG__
#include <iostream>
#endif

#include "communication_protocoll.hh"

using boost::asio::ip::tcp;

class PDPSession:public boost::enable_shared_from_this<PDPSession>{
	public:
	typedef boost::shared_ptr<PDPSession> pointer;

	static pointer create(boost::asio::io_service & io_service){
		return pointer(new PDPSession(io_service));
	}

	tcp::socket & socket(){
		return _socket;
	}

	void start();

	private:

	PDPSession(boost::asio::io_service &io_service)
		:_socket(io_service)
	{
	}

	void async_read(size_t at_least_read_size);
	void handle_read(const boost::system::error_code & error, size_t size);

	inline PackageType get_package_type(){
		return static_cast<PackageType>(*(reinterpret_cast<package_type_t *>(&received_data[0])));
	}

	inline size_t get_data_len(){
		return *(reinterpret_cast<data_len_t *>(&received_data[0] + PackageHeaderFieldLen::TYPE));
	}
	inline size_t get_policy_len(){
		return *(reinterpret_cast<policy_len_t *>(&received_data[0] + PackageHeaderFieldLen::COMMON_HEADER_LEN));
	}

	void update_node_param();
	void eval_policy();

	tcp::socket _socket;
	boost::array<char, 1024> _buf;
	std::vector<char> received_data;
};
