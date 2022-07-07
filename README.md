# E60GWSController
A working controller for the BMW E60 Electronic Gear Selector.
For this, I am using an ESP32 and a ICL CAN Tranceiver on pins 25/26. 
CAN Speed at 500kbp/s.
Uses the default CAN library.
As of right now, it will just cycle thru the gears.
When any button is pressed or when selector is moved it is printed to the console.
iil probably turn this into a gamepad driver and play beamng with it.
if my code is shit make a fork and fix it and make a pull request <3 ty
