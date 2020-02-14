CPPFLAGS:=-std=c++11
CFLAGS:=$(shell root-config --cflags)
LDFLAGS:=
LIBS:=$(shell root-config --libs) $(shell pkg-config freetype2 --libs)
FCOMP = gfortran -c
FLINK = gfortran

######################################################################
MACRO:=dwhi
OBJ = dwhi.o input.o pots.o elastic.o inelastic.o radint_4.o angmom.o coulomb.o subs.o

.PHONY: all
all: $(MACRO)

dwhi: dwhi.o input.o pots.o elastic.o inelastic.o radint_4.o angmom.o coulomb.o subs.o
	$(FLINK) $(OBJ) -o dwhi

dwhi.o:		dwhi.for
	$(FCOMP) dwhi.for
input.o:	input.for
	$(FCOMP) input.for
pots.o:		pots.for
	$(FCOMP) pots.for
elastic.o:	elastic.for
	$(FCOMP) elastic.for
inelastic.o:	inelastic.for
	$(FCOMP) inelastic.for
radint_4.o:	radint_4.for
	$(FCOMP) radint_4.for
angmom.o:	angmom.for
	$(FCOMP) angmom.for
coulomb.o:	coulomb.for
	$(FCOMP) coulomb.for
subs.o:		subs.for
	$(FCOMP) subs.for

.PHONY: clean
clean:
	rm -f *.o *_C.* $(MACRO)