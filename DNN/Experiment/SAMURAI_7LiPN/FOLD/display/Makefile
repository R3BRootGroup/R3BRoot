CPPFLAGS:=
CFLAGS:=$(shell root-config --cflags)
LDFLAGS:=
LIBS:=$(shell root-config --libs) $(shell pkg-config freetype2 --libs)

######################################################################
CLASS1:=ObjDouble
######################################################################

MACRO:=PlotTheFile

.PHONY: all
all: $(MACRO)

$(MACRO): $(MACRO).o $(CLASS1).o dict.o
	g++ -o $@ $^ $(LIBS)

%.o: %.cpp Makefile
	g++ -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

dict.cpp: $(CLASS1).h Makefile
	rootcint -f $@ -c $(filter-out Makefile,$^) LinkDef.h
	
MACRO:=DoubleDisplay

.PHONY: all
all: $(MACRO)

$(MACRO): $(MACRO).o $(CLASS1).o dict.o
	g++ -o $@ $^ $(LIBS)

%.o: %.cpp Makefile
	g++ -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

dict.cpp: $(CLASS1).h Makefile
	rootcint -f $@ -c $(filter-out Makefile,$^) LinkDef.h
	
.PHONY: clean
clean:
	rm -f *.o *_C.* $(MACRO) dict.cpp dict.h
