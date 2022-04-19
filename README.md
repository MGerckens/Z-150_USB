# Z-150 USB
Simple converter program to use a Zenith Z-150 XT (black label) on a modern computer via USB.
Should work on any 32U4-based Arduino, tested with a Pro Micro. Requires no additional hardware besides a few jumper wires.

Pins can be changed via macro definitions, default to:
- Reset -> 6
- Clock -> 7
- Data -> 8

Default button remaps:
- Asterisk -> Windows key
- Numpad minus -> F11
- Numpad plus -> F12
- Shift+alt+numpad -> Media controls (volume up/down, play/pause, previous/next)

The Z-150 XT is slightly off-spec from normal PC-XT keyboards, requiring the reset line to be used and the clock to be pulled high. There are existing converters that work with it, but they're all rather complex to set up and built around the Teensy. This one can be written via the Arduino IDE without compiling or messing with the bootloader.
