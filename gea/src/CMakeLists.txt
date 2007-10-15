
INCLUDE_DIRECTORIES(./include/gea)

IF (GXX_ACCEPTS_VISHIDDEN)
  ADD_DEFINITIONS(-fvisibility=hidden)
ENDIF (GXX_ACCEPTS_VISHIDDEN)

ADD_LIBRARY(gea SHARED 
  EventHandler.cc	
  gea.cc 
  Blocker.cc 
  Handle.cc 
  UdpHandle.cc 
  DependHandle.cc 
  ObjRepository.cc
  )


ADD_LIBRARY(gea-posix SHARED 
  Time.cc 
  UnixFdHandle.cc 
  ShadowBlocker.cc 
  ShadowHandle.cc 
  ShadowEventHandler.cc 
  ShadowUdpHandle.cc 
  ShadowDependHandle.cc 
  NodeColor.cc 
  guess_iface.cc 
  compat.cc 
  AnimLink.cc
  )


SET_TARGET_PROPERTIES(gea gea-posix
  PROPERTIES
  VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
  )
  




INSTALL(TARGETS gea gea-posix LIBRARY DESTINATION lib)

# Local variables:
# mode:cmake
# End:
