CPPFLAGS=-std=c++11 -O3 # -g

cppl_generator:	equation_parser.yy equation_scanner.ll cppl_generator.cc ast policy_definition.o policy_stack.o relation_set.o variable_set.o policy_header.o binary.o debug.o performance.o function_handler.o jsoncpp.o options.hh
	bison -d equation_parser.yy
	flex -o equation_scanner.lex.cc equation_scanner.ll
	g++ $(CPPFLAGS) -o $@ cppl_generator.cc equation_driver.cc equation_parser.tab.cc equation_scanner.lex.cc ast_print_visitor.o ast_preprocessor_visitor.o policy_definition.o policy_stack.o relation_set.o variable_set.o policy_header.o ast_policy_compressor_visitor.o binary.o performance.o function_handler.o debug.o -lboost_program_options -ljsoncpp

ast: ast.hh ast_print_visitor.hh ast_print_visitor.cc ast_preprocessor_visitor.hh ast_preprocessor_visitor.cc ast_policy_compressor_visitor.hh ast_policy_compressor_visitor.cc debug.o
	g++ -c $(CPPFLAGS) ast_print_visitor.cc
	g++ -c $(CPPFLAGS) ast_preprocessor_visitor.cc
	g++ -c $(CPPFLAGS) ast_policy_compressor_visitor.cc

policy_definition.o: policy_definition.hh policy_definition.cc variable_set.o binary.o debug.o
	g++ -c $(CPPFLAGS) policy_definition.cc

relation_set.o: relation_set.hh relation_set.cc variable_set.o binary.o debug.o options.hh
	g++ -c $(CPPFLAGS) relation_set.cc

policy_stack.o: policy_stack.hh policy_stack.cc binary.o debug.o
	g++ -c $(CPPFLAGS) policy_stack.cc

variable_set.o: variable_set.hh variable_set.cc binary.o debug.o
	g++ -c $(CPPFLAGS) variable_set.cc

policy_header.o: policy_header.hh policy_header.cc binary.o debug.o
	g++ -c $(CPPFLAGS) policy_header.cc

binary.o: binary.hh binary.cc debug.o
	g++ -c $(CPPFLAGS) binary.cc

performance.o: performance.h performance.c debug.o
	g++ -c $(CPPFLAGS) performance.c

function_handler.o: function_handler.hh function_handler.cc debug.o
	g++ -c $(CPPFLAGS) function_handler.cc

jsoncpp.o: json/json.h jsoncpp.cpp
	g++ -c $(CPPFLAGS) jsoncpp.cpp

debug.o: debug.hh debug.cc
	g++ -c $(CPPFLAGS) debug.cc

clean:
	rm cppl_generator equation_scanner.lex.cc equation_parser.tab.cc equation_parser.tab.hh position.hh location.hh stack.hh *.o
