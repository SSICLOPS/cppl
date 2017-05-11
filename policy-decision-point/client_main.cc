#include <boost/asio.hpp>

#include "client.hh"

using boost::asio::ip::tcp;

int main(void){
	boost::asio::io_service io_service;

	ClientSessionParam csp = {"Hello World!", "../examples/example_paper/policy_compressed.ccppl"};

	Client c(io_service, "127.0.0.1", "12345", csp);

	io_service.run();
}
