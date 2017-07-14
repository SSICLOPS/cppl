
SOURCES = cpplmodule.cc cppl.py
OBJFILES = 

MODULES =

cppl.so: build

# TODO clean dependency ensures that always the newest ../../libprivercloud.a is used
build: clean setup.py $(SOURCES) $(OBJFILES) $(MODULES) libcppl
	python setup.py build

sdist: $(SOURCES) setup.py $(OBJFILES) libcppl
	@echo "WARNING: untested makefile target"
	python setup.py sdist

install: $(SOURCES) setup.py $(OBJFILES) libcppl
	@echo "WARNING: untested makefile target"
	python setup.py install

.PHONY: libcppl
libcppl:
	make -C ../policy-decision-point/ lib

tidy:
	rm -fr build

clean: tidy

