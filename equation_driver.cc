#include "equation_driver.hh"

EquationDriver::EquationDriver()
    : trace_scanning (false), trace_parsing (false)
{
}

EquationDriver::~EquationDriver()  {
}

void EquationDriver::parse(const std::string &f) {
    file = f;
    scan_begin();
    yy::EquationParser parser(*this);
    parser.set_debug_level(trace_parsing);
    assert(parser.parse() == 0);
    scan_end();
}

void EquationDriver::error(const yy::location& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
    exit(EXIT_FAILURE);
}

void EquationDriver::error(const std::string& m) {
    std::cerr << m << std::endl;
    exit(EXIT_FAILURE);
}
