#	$NetBSD: Makefile,v 1.25 1997/08/22 21:53:00 perry Exp $
#
# WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
# This boot program is obsolete. It has been left in place for
# reference only. It doesn't even build any longer. Please use the
# boot blocks in sys/arch/i386/stand instead!
# WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
#
# Ported to boot 386BSD by Julian Elischer (julian@tfs.com)
# September 1992
#
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
#
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
#
# Carnegie Mellon requests users of this software to return to
#
#  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
#
# any improvements or extensions that they make and grant Carnegie Mellon
# the rights to redistribute these changes.
#
# WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
# This boot program is obsolete. It has been left in place for
# reference only. It doesn't even build any longer. Please use the
# boot blocks in sys/arch/i386/stand instead!
# WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING

all:
	@echo "This boot program is obsolete. It has been left in place for"
	@echo "reference only. It doesn't even build any longer. Please use"
	@echo "the boot blocks in sys/arch/i386/stand instead."


S=	${.CURDIR}/../../..

.PHONY:	machine-links
machine-links:
	-rm -f machine && \
	    ln -s ${.CURDIR}/../include machine
	-rm -f ${MACHINE_ARCH} && \
	    ln -s ${.CURDIR}/../include ${MACHINE_ARCH}

real-all: machine-links biosboot bootbios

NOPROG=	noprog
NOMAN=	noman

CFLAGS=	-O6 -D_KERNEL -DI386_CPU -DI486_CPU -DI586_CPU
CFLAGS+=-DDO_BAD144 -I. -I${.CURDIR} -I$S -I${.CURDIR}/../..

# Uncomment this to make the boot block talk to a serial port.
#CPPFLAGS+=-DSERIAL

# start.o should be first
OBJS=	start.o table.o boot.o asm.o bios.o io.o disk.o sys.o version.o

boot: ${OBJS}
	${LD} -Bstatic -e start -N -T 0 -o boot ${OBJS}
	cp boot boot.sym
	@strip boot
	@sh ${.CURDIR}/rmaouthdr boot boot.tmp
	@mv -f boot.tmp boot
	@ls -l boot

biosboot: boot
	dd if=boot of=biosboot count=1

bootbios: boot
	dd if=boot of=bootbios skip=1

install: biosboot bootbios
	cp biosboot ${DESTDIR}/usr/mdec/sdboot
	cp bootbios ${DESTDIR}/usr/mdec/bootsd
	rm -f ${DESTDIR}/usr/mdec/wdboot
	ln ${DESTDIR}/usr/mdec/sdboot ${DESTDIR}/usr/mdec/wdboot
	rm -f ${DESTDIR}/usr/mdec/bootwd
	ln ${DESTDIR}/usr/mdec/bootsd ${DESTDIR}/usr/mdec/bootwd
	rm -f ${DESTDIR}/usr/mdec/fdboot
	ln ${DESTDIR}/usr/mdec/sdboot ${DESTDIR}/usr/mdec/fdboot
	rm -f ${DESTDIR}/usr/mdec/bootfd
	ln ${DESTDIR}/usr/mdec/bootsd ${DESTDIR}/usr/mdec/bootfd

CLEANFILES+=biosboot bootbios boot boot.sym machine ${MACHINE_ARCH}

.include <bsd.prog.mk>
