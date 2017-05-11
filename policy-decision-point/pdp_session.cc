#include <boost/thread.hpp>
#include "pdp_session.hh"
#include "node_parameter_manager.hh"
#include "policy_stack.hh"

void PDPSession::start(){
	async_read(PackageHeaderFieldLen::COMMON_HEADER_LEN);
}

void PDPSession::async_read(size_t at_least_read_size){
	boost::asio::async_read(_socket, boost::asio::buffer(_buf),
			boost::asio::transfer_at_least(at_least_read_size),
			boost::bind(&PDPSession::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
}

void PDPSession::handle_read(const boost::system::error_code & error, size_t bytes_transferred){
	if (!error){
#ifdef __DEBUG__
		std::cout<<"bytes transfered:"<< bytes_transferred << std::endl;
#endif
		size_t bytes_to_read = 0;

		received_data.insert(received_data.end(),
				_buf.c_array(),
				_buf.c_array() + bytes_transferred);

		PackageType packageType = get_package_type();
		if (packageType == PackageType::NODE_PARAM){
			bytes_to_read = PackageHeaderFieldLen::COMMON_HEADER_LEN
				+ get_data_len()
				- received_data.size();

			if (bytes_to_read <= 0)
				update_node_param();
			else
				async_read(bytes_to_read);
		}
		else if (packageType == PackageType::POLICY_ENABLED_DATA){
			if (received_data.size() < PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN){
				bytes_to_read = PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN
					+ get_data_len()
					- received_data.size();

				async_read(bytes_to_read);
			}
			else if (received_data.size() < (bytes_to_read = 
											(PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN
					                         + get_data_len()
											 + get_policy_len())))
			{
				bytes_to_read -= received_data.size(); 
				
				async_read(bytes_to_read);
			}
			else{
				eval_policy();
			}
		}
	}
}

void __upate_node_param(const std::string ip, std::vector<char> buf){
#ifdef __DEBUG__
	std::cout<<"__update_node_param"<<std::endl;
	std::cout<<"from thread: "<<boost::this_thread::get_id()<<std::endl;
	std::cout<<"ip: "<<ip<<std::endl;
	std::cout<<std::string(buf.begin() + PackageHeaderFieldLen::COMMON_HEADER_LEN, buf.end())<<std::endl;
#endif
	NodeParameterManager * nodeParameterManager = NodeParameterManager::getInstance();
	nodeParameterManager->addParamsString(ip, std::string(buf.begin() + PackageHeaderFieldLen::COMMON_HEADER_LEN, buf.end()));
}

void PDPSession::update_node_param(){
#ifdef __DEBUG__
	std::cout<<"__update_node_param"<<std::endl;
	std::cout<<"from thread: "<<boost::this_thread::get_id()<<std::endl;
	std::cout<<std::string(_buf.begin() + PackageHeaderFieldLen::COMMON_HEADER_LEN, _buf.end())<<std::endl;
#endif
	boost::thread t(__upate_node_param, _socket.remote_endpoint().address().to_string(), std::move(received_data));
	t.detach();
}

void __eval_policy(std::vector<char> buf){
#ifdef __DEBUG__
	std::cout<<"eval_policy"<<std::endl;
	std::cout<<"from thread: "<<boost::this_thread::get_id()<<std::endl;
	std::cout<<"bytes received: "<<buf.size()<<std::endl;
	std::cout<<std::string(buf.begin() + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN
			, buf.begin() + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN
			+ *reinterpret_cast<data_len_t *>(&buf[0] + PackageHeaderFieldLen::TYPE))<<std::endl;
	std::cout<<"policy len:"<< *reinterpret_cast<policy_len_t *>(&buf[0] + PackageHeaderFieldLen::COMMON_HEADER_LEN)<<std::endl;
#endif
	const NodeParameters * pnp = NULL;
	PolicyStack policyStack(&pnp);
	policyStack.load(&buf[0] + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN + *reinterpret_cast<data_len_t *>(&buf[0] + PackageHeaderFieldLen::TYPE),
			(*reinterpret_cast<policy_len_t *>(&buf[0] + PackageHeaderFieldLen::COMMON_HEADER_LEN))<<3);//<<3 := *8

	NodeParameterManager * nodeParameterManager = NodeParameterManager::getInstance();

	std::vector<std::string> serverList = nodeParameterManager->getServerList();
	for (auto it = serverList.begin(); it != serverList.end(); ++it){
		NodeParameterManagerNode * npmn = nodeParameterManager->getNodeParameters(*it, policyStack.getVersion());
		boost::shared_lock<boost::shared_mutex> lock(npmn->_mutex);
		pnp = npmn->_nodeParameters;
		policyStack.doEval();
		std::cout<< *it <<": "<<((policyStack.getResult())?"true":"false")<<std::endl;
	}
}

void PDPSession::eval_policy(){
#ifdef __DEBUG__
	std::cout<<"eval_policy"<<std::endl;
	std::cout<<"from thread: "<<boost::this_thread::get_id()<<std::endl;
	std::cout<<std::string(_buf.begin() + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN
			, _buf.begin() + PackageHeaderFieldLen::POLICY_ENABLED_DATA_HEADER_LEN
			+ *reinterpret_cast<data_len_t *>(&_buf[0] + PackageHeaderFieldLen::TYPE))<<std::endl;
#endif
	boost::thread t(__eval_policy, std::move(received_data));
	t.detach();
}
