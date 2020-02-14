CPPFLAGS:=-std=c++11
CFLAGS:=$(shell root-config --cflags)
LDFLAGS:=
LIBS:=$(shell root-config --libs) $(shell pkg-config freetype2 --libs)
FCOMP = gfortran -c
FLINK = gfortran

######################################################################
MACRO:=fold
OBJ = fold.o input.o amp.o density.o direct.o exchng.o inter.o efold.o angmom.o bessel.o moshin.o subs.o

.PHONY: all
all: $(MACRO)

fold: fold.o input.o amp.o density.o direct.o exchng.o inter.o efold.o angmom.o bessel.o moshin.o subs.o
	$(FLINK) $(OBJ) -o fold

fold.o:		fold.for
	$(FCOMP) fold.for
input.o:	input.for
	$(FCOMP) input.for
amp.o:		amp.for
	$(FCOMP) amp.for
density.o:	density.for
	$(FCOMP) density.for
direct.o:	direct.for
	$(FCOMP) direct.for
exchng.o:	exchng.for
	$(FCOMP) exchng.for
inter.o:	inter.for
	$(FCOMP) inter.for
efold.o:	efold.for
	$(FCOMP) efold.for
angmom.o:	angmom.for
	$(FCOMP) angmom.for
bessel.o:	bessel.for
	$(FCOMP) bessel.for
moshin.o:	moshin.for
	$(FCOMP) moshin.for
subs.o:		subs.for
	$(FCOMP) subs.for

.PHONY: clean
clean:
	rm -f *.o *_C.* $(MACRO)