#
# A template makefile for the MPI assignment.
# Concurrent and Distributed Programming Course, spring 2014.
# Faculty of Mathematics, Informatics and Mechanics.
# University of Warsaw, Warsaw, Poland.
#
# Copyright (C) Piotr Sokolski, 2014.
#

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	MPICC = g++
	LDFLAGS =  -stdlib=libc++ -lmpi_cxx -lmpi
else
	MPICC = mpicc -cc=/bgsys/drivers/V1R4M2_200_2010-100508P/ppc/gnu-linux/powerpc-bgp-linux/bin/g++
	LDFLAGS =
endif

CCFLAGS =

MATGEN_TYPE ?= matgen-mt
MATGEN_FILE = $(MATGEN_TYPE).o 

all: msp-seq-naive.exe msp-par.exe

msp-par.exe: msp-par.o common.o $(MATGEN_FILE)
	$(MPICC) -O3 -o $@ $(LDFLAGS) $^

msp-seq-naive.exe: msp-seq-naive.o $(MATGEN_FILE)
	$(MPICC) -O3 -o $@ $(LDFLAGS) $^

%.o: %.c matgen.h common.h Makefile
	$(MPICC) $(CCFLAGS) -O3 -c -o $@ $<

clean:
	rm -f *.o *core *~ *.out *.err *.exe

