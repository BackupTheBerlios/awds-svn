CPPFLAGS += -I../../include

bin_PROGRAMS = gea_start

lib_LTLIBRARIES = libgea2-utils.la

gea_start_SOURCES = gea_start.cc
gea_start_CPPFLAGS = $(LTDLINCL)
gea_start_LDADD = ../libgea2.la ../libgea2-posix.la -ldl

libgea2_utils_la_SOURCES = \
	SingleShootEvent.cc \
	MultiShootEvent.cc \
	MultiShootTimerEvent.cc

#libgea_utils_la_CPPFLAGS = -fPIC
#libgea_utils_la_LDFLAGS = -static 

