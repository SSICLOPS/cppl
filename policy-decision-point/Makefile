USE_SYSTEM_JSONCPP = 1

CC := g++
#CPPFLAG := -std=c++11 -g -D __DEBUG__
CPPFLAG := -std=c++11 -O3

OBJS := binary.o ccppl_decompressor.o node_parameter.o pdp_session.o policy_definition.o policy_stack_evaluator.o relation_set.o

ifeq ($(USE_SYSTEM_JSONCPP), 0)
OBJS += ../jsoncpp.o
LIBHSONCPP :=
else
LIBJSONCPP := -ljsoncpp
endif

all:server client policy_decision_point lib

lib:func_handler_23.so

server: server_main.cc tcp_client.hh server_session.hh server.hh
	$(CC) $(CPPFLAG) -o $@ $< -lboost_system -lboost_thread -lpthread
client: client_main.cc tcp_client.hh client_session.hh client.hh
	$(CC) $(CPPFLAG) -o $@ $< -lboost_system -lboost_thread -lpthread
policy_decision_point: policy_decision_point_main.cc tcp_server.hh pdp_session.o policy_decision_point.hh $(OBJS)
	$(CC) $(CPPFLAG) -o $@ $< $(OBJS) -lboost_system -lboost_thread -lpthread $(LIBJSONCPP) -ldl

func_handler_23.so:func_handler_23.cc node_parameter.cc
	$(CC) $(CPPFLAG) -c -fPIC -o func_handler_23.o $<
	$(CC) $(CPPFLAG) -c -fPIC -o node_parameter.o node_parameter.cc
	$(CC) -shared -o $@ func_handler_23.o node_parameter.o
	rm -f func_handler_23.o node_parameter.o

.SECONDARY:$(OBJECTS)
%.o: %.cc %.hh
	$(CC) $(CPPFLAG) -c $(CPPFLAGS) $<

clean:
	rm -f server client policy_decision_point *.so *.o
