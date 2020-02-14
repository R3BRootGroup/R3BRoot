CPPFLAGS:=-std=c++11
CFLAGS:=$(shell root-config --cflags)
LDFLAGS:=
LIBS:=$(shell root-config --libs) $(shell pkg-config freetype2 --libs)
FCOMP = gfortran -c
FLINK = gfortran

######################################################################
MACRO:=wsaw

.PHONY: all
all: $(MACRO)

# C++ compilation:
# $(MACRO): $(MACRO).o
#	g++ -o $@ $^ $(LIBS)

# $(MACRO).o: $(MACRO).cpp Makefile
#	g++ -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

# Fortran compilation:
$(MACRO): $(MACRO).o
	$(FLINK) $(MACRO).o -o $(MACRO)

$(MACRO).o: $(MACRO).for 
	$(FCOMP) $(MACRO).for

.PHONY: clean
clean:
	rm -f *.o *_C.* $(MACRO)