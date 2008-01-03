#include "gea/utils/MultiShootEvent.h"

using namespace gea;

void MultiShootEvent::event(Handle *h, AbsTime t, void* data) {
  MultiShootEvent *self = reinterpret_cast<MultiShootEvent *>(data);

  self->alive = false;
  self->handle(h, t);
  if (!self->alive) delete self;
}
