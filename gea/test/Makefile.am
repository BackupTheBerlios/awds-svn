CPPFLAGS += -I../include

GEAS = periodic.gea \
	blink.gea \
	unixfd.gea \
	dependency.gea \
	broadcast.gea \
	HidSt.gea \
	HidStRecv.gea \
	HidSt2.gea \
	objrepos2.gea \
	objrepos.gea



check_PROGRAMS = periodic blink unixfd broadcast

periodic_SOURCES = gea_stub.cc
periodic_LDADD =  periodic.la  ../src/libgea-posix.la 
periodic_CXXFLAGS = $(AM_CFLAGS) 

AM_LDFLAGS = -module -export-dynamic -avoid-version --enable-auto-import

check_LTLIBRARIES = $(GEAS:.gea=.la)

periodic_la_SOURCES = periodic.cc

blink_la_SOURCES = blink.cc 
blink_SOURCES = gea_stub.cc
blink_LDADD = blink.la  ../src/libgea-posix.la 

dependency_la_SOURCES = dependency.cc

unixfd_la_SOURCES = unixfd.cc
unixfd_SOURCES = gea_stub.cc
unixfd_LDADD =  unixfd.la ../src/libgea-posix.la 

broadcast_la_SOURCES = broadcast.cc
broadcast_SOURCES = gea_stub.cc 
broadcast_LDADD = blink.la ../src/libgea-posix.la 


HidSt_la_SOURCES = HidSt.cc

HidStRecv_la_SOURCES = HidStRecv.cc

HidSt2_la_SOURCES = HidSt2.cc

# Testing_la_SOURCES = Testing.cc

# test_la_SOURCES = test.cc

objrepos_la_SOURCES = objrepos.cc

objrepos2_la_SOURCES = objrepos2.cc


