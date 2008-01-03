** time representation **

The time is represented by two classes:
AbsTime and Duration

The first one defines a fix point in time. The latter represents the difference of
two absolute times. 

How it works:

There is a class for the event handling: 

gea::EventHandler

it allows you to wait for an event with 

void waitFor(Handle *h, AbsTime timeout, Event e, void *data);

The Handle normally represents something that can emmit events, i.e. a unix file
descriptor. For every waiting you have to define a timeout. This timeout is an absolut time. 
Whe the event or the timout occurs the eventhandler e is called with the handle, the time of
the event and the data as parameters. 

TODO ... Maybe we should use doxygen for this.







