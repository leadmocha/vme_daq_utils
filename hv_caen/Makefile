#
# File:
#    Makefile
#
# Description:
#    Makefile for executable to control the CAEN V6533N module via
#    VMEbus using a Intel-Based Controller running Linux.
#
LINUXVME_BIN	?= .
LINUXVME_LIB	?= ../lib
LINUXVME_INC	?= ../include

CROSS_COMPILE		=
CC			= $(CROSS_COMPILE)gcc
AR                      = ar
RANLIB                  = ranlib
CFLAGS			= -Wall -g -I${LINUXVME_INC} -I. -I/usr/include \
			  -L${LINUXVME_LIB} -L. #-I${VMEDIR}/caenv6533

PROGS			= hvcli

all: $(PROGS)

clean distclean:
	@rm -f $(PROGS) *~ *.so

%: %.c
	$(CC) $(CFLAGS) -o $@ $(@:%=%.c) -ljvme -ldl -lcaenv6533

install: $(PROGS)
	cp -v $(PROGS) $(LINUXVME_BIN)/

links: ${PROGS}
	for file in $(PROGS); do \
	ln -sf ${PWD}/$${file} ${LINUXVME_BIN}/ ; \
	done

.PHONY: all clean distclean
