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



#check_PROGRAMS = periodic blink unixfd broadcast



AM_LDFLAGS = -module -export-dynamic -avoid-version 

foodir=/tmp/
foo_LTLIBRARIES = $(GEAS:.gea=.la) time2array.la

time2array_la_SOURCES = time2array.cc
time2array_la_LIBADD = -lgea

periodic_la_SOURCES = periodic.cc

blink_la_SOURCES = blink.cc 

dependency_la_SOURCES = dependency.cc

unixfd_la_SOURCES = unixfd.cc

broadcast_la_SOURCES = broadcast.cc


HidSt_la_SOURCES = HidSt.cc

HidStRecv_la_SOURCES = HidStRecv.cc

HidSt2_la_SOURCES = HidSt2.cc

# Testing_la_SOURCES = Testing.cc

# test_la_SOURCES = test.cc

objrepos_la_SOURCES = objrepos.cc

objrepos2_la_SOURCES = objrepos2.cc


