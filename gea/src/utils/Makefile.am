CPPFLAGS += -I../../include


bin_PROGRAMS = gea_start

lib_LTLIBRARIES = libgea-utils.la

gea_start_SOURCES = gea_start.cc
gea_start_CPPFLAGS = $(LTDLINCL)
gea_start_LDADD = ../libgea-posix.la 
gea_start_LDFLAGS = $(LIBLTDL)  -dlopen self 


libgea_utils_la_SOURCES = \
	SingleShootEvent.cc \
	MultiShootEvent.cc \
	MultiShootTimerEvent.cc

#libgea_utils_la_CPPFLAGS = -fPIC
#libgea_utils_la_LDFLAGS = -static 

