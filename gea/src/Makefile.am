CPPFLAGS += "-I../include"

lib_LTLIBRARIES = libgea-posix.la

libgea_posix_la_SOURCES = \
	EventHandler.cc \
	gea.cc \
	Blocker.cc \
	Time.cc \
	UdpHandle.cc \
	Handle.cc \
	DependHandle.cc \
	UnixFdHandle.cc \
	ShadowEventHandler.cc \
	ShadowUdpHandle.cc \
	NodeColor.cc \
	guess_iface.cc \
	ObjRepository.cc \
	compat.cc \
	AnimLink.cc

libgea_posix_la_CPPFLAGS = -DGEA_POSIX 

libgea_posix_la_LDFLAGS =   @NETWORK_LIBS@ -export-dynamic -version-info @MAJOR_VER@:@MINOR_VER@:0

#libgea_posix_la_LDADD = 

SUBDIRS = . utils
