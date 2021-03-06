# platform_sunos5_centerline_orbix.GNU,v 4.5 1997/09/10 16:47:28 levine Exp

# SunOS 5.x (Solaris 2.x) with Centerline C++

debug = 1

CC		= cc
CXX		= CC
CFLAGS          += -mt
CCFLAGS         += $(CFLAGS)
DCFLAGS		+= -g
DLD		= $(CXX) -mt 
LD		= $(CXX) -mt
INCLDIRS	= -I$(ORBIX_ROOT)/include
LDFLAGS		+= -L$(ORBIX_ROOT)/lib
LIBS		+= -lITsrv -lsocket -ldl -lnsl -lgen
PIC		= -PIC
AR		= CC
ARFLAGS		= -xar -o
RANLIB		= echo
SOFLAGS		= -G $(CPPFLAGS) $(PTDIRS)
SOBUILD		= $(COMPILE.cc) $(PIC) -o $(VSHDIR)$*.o $<; \
		  $(SOLINK.cc) $(PIC) -o $@ $(LDFLAGS) $(VSHDIR)$*.o
