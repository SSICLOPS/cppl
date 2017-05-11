#include <boost/asio.hpp>
#include "policy_decision_point.hh"

using boost::asio::ip::tcp;

int main(void){
	boost::asio::io_service io_service;

	PolicyDecisionPoint pdp(io_service, 12345);

	io_service.run();
}
