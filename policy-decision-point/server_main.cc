#include <boost/asio.hpp>

#include "server.hh"

using boost::asio::ip::tcp;

int main(void){
	boost::asio::io_service io_service;
	ServerSessionParam ssp = {"../examples/example_paper/policy_nodeparams_1.json", "../examples/example_paper/policy_noderuntimeparams_1.json"};
	
	Server s(io_service, "127.0.0.1", "12345", ssp);

	io_service.run();
}
