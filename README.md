#Installation:
* To run in debug mode use the command "make run"
* To install for startup use the command "make install"
* To uninstall use the command "make uninstall"

#License:
* Copyright 2019 Vincent Mattingly / Regular Tetragon
* MIT license provided in the LICENSE.md file

#Purpose:
* To provide proper controller support for the PiGrrl gamepad as opposed to keyboard emulation with the addition of an ADS1115 to support analog sticks.

#Software Requirements:
* Raspbian Stretch 9 or higher (will likely work in lower versions)
* Installation of wiringPi 2.46 or higher (will work in any version with ADS1115 support)

#Hardware Requirements:
* Any raspberry pi which supports I2C
* An ADS1115 attached to I2C header with address 0x48
* Button wirings corresponding to the PiGrrl 2 gamepad board
* A PiTFT with the 4 provided buttons
