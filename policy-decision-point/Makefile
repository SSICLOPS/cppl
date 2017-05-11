USE_SYSTEM_JSONCPP = 1

CC := g++
#CPPFLAGS := -std=c++11 -g -D __DEBUG__
CPPFLAGS := -std=c++11 -O3

#OBJS := binary.o ccppl_decompressor.o node_parameter.o pdp_session.o policy_definition.o policy_stack_evaluator.o relation_set.o
OBJS := binary.o policy_definition.o node_parameter.o ccppl_decompressor.o policy_stack_evaluator.o relation_set.o simple_reason_printer.o false_reason_printer.o dnf_convertor.o performance.o

COMPRESS_OBJS := ast_preprocessor_visitor.o\
				 equation_parser.tab.o\
				 equation_scanner.lex.o\
				 equation_driver.o\
				 ast_2_policy_stack.o\
				 ast_2_ccppl.o\
				 ccppl_compressor.o

OBJS += $(COMPRESS_OBJS)

ifeq ($(USE_SYSTEM_JSONCPP), 0)
OBJS += jsoncpp.o
LIBJSONCPP :=
else
LIBJSONCPP := -ljsoncpp
endif


#test_main:test_main.cc binary.o policy_stack_evaluator.o node_parameter.o policy_definition.o ccppl_decompressor.o relation_set.o
#	$(CC) $(CPPFLAGS) -o $@ $^ -ldl -lboost_system -lboost_thread -lpthread $(LIBJSONCPP) -L ./ -lcppl

#all:server client policy_decision_point lib
all:cppl_compress cppl_evaluate func_handler_23.so server client policy_decision_point

lib:func_handler_23.so libcppl.so

cppl_compress:cppl_compress.cc libcppl.so
	$(CC) $(CPPFLAGS) -o $@ $< -lboost_program_options $(LIBJSONCPP) -L ./ -lcppl -ldl

cppl_evaluate:cppl_evaluate.cc libcppl.so
	$(CC) $(CPPFLAGS) -o $@ $< -lboost_program_options $(LIBJSONCPP) -L ./ -lcppl -ldl
	
server: server_main.cc tcp_client.hh server_session.hh server.hh
	$(CC) $(CPPFLAGS) -o $@ $< -lboost_system -lboost_thread -lpthread

client: client_main.cc tcp_client.hh client_session.hh client.hh
	$(CC) $(CPPFLAGS) -o $@ $< -lboost_system -lboost_thread -lpthread

policy_decision_point: policy_decision_point_main.cc tcp_server.hh pdp_session.o policy_decision_point.hh libcppl.so
	$(CC) $(CPPFLAGS) -o $@ $< pdp_session.o -lboost_system -lboost_thread -lpthread $(LIBJSONCPP) -L ./ -lcppl -ldl

equation_driver.o:equation_driver.cc equation_driver.hh equation_parser.tab.cc
	$(CC) $(CPPFLAGS) -c -fPIC $<

equation_parser.tab.o:equation_parser.tab.cc
	$(CC) $(CPPFLAGS) -c -fPIC $<

equation_scanner.lex.o:equation_scanner.lex.cc
	$(CC) $(CPPFLAGS) -c -fPIC $<

equation_parser.tab.cc: equation_parser.yy
	bison -d equation_parser.yy

equation_scanner.lex.cc: equation_scanner.ll
	flex -o equation_scanner.lex.cc equation_scanner.ll

performance.o: performance.c performance.h
	$(CC) $(CPPFLAGS) -c -fPIC $<

jsoncpp.o: ../jsoncpp.cpp
	$(CC) $(CPPFLAGS) -c -fPIC $<

libcppl.so:libcppl.cc libcppl.hh $(OBJS)
	$(CC) $(CPPFLAGS) -c -fPIC -o libcppl.o $<
	$(CC) -shared -o $@ libcppl.o $(OBJS)
	rm -f libcppl.o

func_handler_23.so:func_handler_23.cc node_parameter.cc
	$(CC) $(CPPFLAGS) -c -fPIC -o func_handler_23.o $<
	$(CC) $(CPPFLAGS) -c -fPIC -o node_parameter.o node_parameter.cc
	$(CC) -shared -o $@ func_handler_23.o node_parameter.o
	rm -f func_handler_23.o

.SECONDARY:$(OBJECTS)
%.o: %.cc %.hh
	$(CC) $(CPPFLAGS) -c -fPIC $<

clean:
	rm -f server client policy_decision_point *.so *.o *.a *.ccppl
	rm -f *.out
	rm -f cppl_compress cppl_evaluate
	rm -f equation_parser.tab.hh equation_parser.tab.cc
	rm -f equation_scanner.lex.cc
