```
int: tempo
int: sample rate
double: song length (seconds)
int: device count
 list of devices:
  byte: device id
  int: chunk size
  byte[]: chunk data
int: midi lane count
 list of midi lanes:
 int: event count
 list of events:
  int: samples from last event (or from 0 if first event)
  byte: msb is type (on or off) remaining 7 bits = note
  byte: velocity (only present if type is note on)
int: track count
list of tracks (in calculation order):
 float: track volume
 int: receive count (including original children)
 list of receives:
  int: sending track index
  int: receiving channel index
  float: volume
 int: device count
 list of device indexes:
  int: device index
 int: midi lane id
 int: automation count
 list of automations:
  int: device index
  int: param id
  int: point count
  list of points:
   int: samples from last point (or from 0 if first point)
   byte: quantized value (0-255)

effect id's:
00: Falcon
01: Slaughter
02: Thunder
03: Scissor
04: Leveller
05: Crusher
06: Echo
07: Smasher
08: Chamber
09: Twister
10: Cathedral
11: Adultery
12: Specimen

event types:
0: Note on
1: Note off
```