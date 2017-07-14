USE_SYSTEM_JSONCPP = 1

CC := g++
CPPFLAGS := -std=c++11 -O3 
#CPPFLAGS += -gdwarf-4 -fvar-tracking-assignments
COMMON_OBJS := policy_definition.o\
			   policy_header.o\
			   policy_stack.o\
			   relation_set.o\
			   variable_set.o\
			   function_handler.o\
			   binary.o\
			   performance.o\
			   debug.o\
			   policy_stack_processor.o

ifeq ($(USE_SYSTEM_JSONCPP), 0)
COMMON_OBJS += jsoncpp.o
LIBHSONCPP :=
else
LIBJSONCPP := -ljsoncpp
endif

COMPRESS_OBJS := ast_print_visitor.o\
				 ast_preprocessor_visitor.o\
				 ast_policy_compressor_visitor.o

EVAL_OBJS := simple_reason_printer.o false_reason_printer.o dnf_convertor.o

OBJECTS := $(COMMON_OBJS) $(COMPRESS_OBJS) $(EVAL_OBJS)

LIB := -lboost_program_options $(LIBJSONCPP)

DOT_CC := equation_parser.tab.cc equation_scanner.lex.cc equation_driver.cc

RM := rm -f

MAKE := make

all: cppl_generator cppl_compress cppl_evaluate policy-decision-point

cppl_generator: cppl_generator.cc $(DOT_CC) $(OBJECTS)
	$(CC) $(CPPFLAGS) -o $@ $^ $(LIB)

cppl_evaluate: cppl_evaluate.cc $(COMMON_OBJS) $(EVAL_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ $(LIB)

cppl_compress: cppl_compress.cc $(DOT_CC) $(COMMON_OBJS) $(COMPRESS_OBJS)
	$(CC) $(CPPFLAGS) -o $@ $^ $(LIB)

equation_parser.tab.cc: equation_parser.yy
	bison -d equation_parser.yy

equation_scanner.lex.cc: equation_scanner.ll
	flex -o equation_scanner.lex.cc equation_scanner.ll

jsoncpp.o: jsoncpp.cpp json/json.h
	$(CC) -c $(CPPFLAGS) $<

performance.o: performance.c performance.h
	$(CC) -c $(CPPFLAGS) $<

.SECONDARY:$(OBJECTS)
%.o: %.cc %.hh options.hh debug.hh
	$(CC) -c $(CPPFLAGS) $<

.PHONY: policy-decision-point
policy-decision-point:
	$(MAKE) -C policy-decision-point

.PHONY: python-module
python-module:
	$(MAKE) -C python-module

test:
	make -C examples/test_cases/
	make -C examples/test_cases/ clean

clean:
	$(RM) cppl_generator cppl_compress cppl_evaluate
	$(RM) equation_parser.tab.hh equation_parser.tab.cc
	$(RM) equation_scanner.lex.cc
	$(RM) position.hh location.hh stack.hh
	$(RM) *.o
	#delete output from reason printer
	$(RM) *.json
	$(MAKE) clean -C policy-decision-point
	$(MAKE) clean -C python-module
