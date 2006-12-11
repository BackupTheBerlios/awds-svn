#include "gea/utils/SingleShootEvent.h"

using namespace gea;

void SingleShootEvent::event(Handle *h, AbsTime t, void* data) {
  SingleShootEvent *self = reinterpret_cast<SingleShootEvent *>(data);

  self->handle(h, t);
  delete self;
}

void SingleShootEvent::wait(AbsTime timeout) {
  GEA.waitFor(wh, timeout, SingleShootEvent::event, this);
}
