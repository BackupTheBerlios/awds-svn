

INSTALL(FILES
  AnimLink.h
  API.h
  ApiIface.h
  Blocker.h
  compat.h
  config.h
  DependHandle.h
  EventHandler.h
  gea_main.h
  Handle.h
  NodeColor.h
  ObjRepository.h
  Time.h
  UdpHandle.h
  DESTINATION include/gea
  COMPONENT development
)


INSTALL(FILES 
  posix/FTdlclose.h
  posix/PosixApiIface.h
  posix/ShadowBlocker.h
  posix/ShadowDependHandle.h
  posix/ShadowEventHandler.h
  posix/ShadowHandle.h
  posix/ShadowUdpHandle.h
  posix/UnixFdHandle.h
  DESTINATION include/gea/posix
  COMPONENT development
)
