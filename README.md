# Z-150 USB
Simple converter program to use a Zenith Z-150 XT (black label) on a modern computer via USB.
Tested on a Pro Micro, should work on any 32U4-based Arduino. Requires no additional hardware besides a few jumper wires.

Pins can be changed via macro definitions, default to:
- Reset -> 6
- Clock -> 7
- Data -> 8

I remapped the asterisk button to the Windows key for taking screenshots easily, other mappings can be added in the same switch statement. May be useful if you need F11 or F12.
