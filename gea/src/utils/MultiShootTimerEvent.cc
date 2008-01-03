#include "gea/utils/MultiShootTimerEvent.h"
#include <stdlib.h>

using namespace gea;

void MultiShootTimerEvent::event(Handle *h, AbsTime t, void* data) {
  MultiShootTimerEvent *self = reinterpret_cast<MultiShootTimerEvent *>(data);

  self->alive = false;
  self->handle(h, t);
  if (!self->alive) delete self;
  else {
    Duration d(self->duration);
    if (self->jitter) d = self->duration + ((rand()%self->jitter)-self->jitter/2)*self->jitterbase;
    GEA.waitFor(h, t + self->duration, MultiShootTimerEvent::event, data);
  }
}
