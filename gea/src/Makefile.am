CPPFLAGS += "-I../include"

lib_LTLIBRARIES = libgea-posix.la libgea.la

libgea_la_SOURCES = \
	EventHandler.cc \
	gea.cc \
	Blocker.cc \
	Handle.cc \
	UdpHandle.cc \
	DependHandle.cc \
	ObjRepository.cc


libgea_posix_la_SOURCES = \
	Time.cc \
	UnixFdHandle.cc \
	ShadowBlocker.cc \
	ShadowHandle.cc \
	ShadowEventHandler.cc \
	ShadowUdpHandle.cc \
	ShadowDependHandle.cc \
	NodeColor.cc \
	guess_iface.cc \
	compat.cc \
	AnimLink.cc

libgea_posix_la_CPPFLAGS = -DGEA_POSIX 

libgea_posix_la_LDFLAGS = @NETWORK_LIBS@ -version-info @MAJOR_VER@:@MINOR_VER@:0

libgea_la_LDFLAGS = -version-info @MAJOR_VER@:@MINOR_VER@:0

SUBDIRS = . utils
