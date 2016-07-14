# arduino-garagedoor
Arduino garage door opener based on fingerprint scanner

This is a trivial sketch that shows a real world application
of the fingerprint scanner that can be for example here:

## Use

1. Ensure you have Arduino version 1.6 or above.
1. Download and install the [library](https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library) from Adafruit.
1. Load and compile the sketch

## Hardware

I used the following hardware:

1. Fingerprint scanner: https://www.adafruit.com/products/751
1. Arduino Micro Pro (5V, 328p, 16MHz)
1. Relais (search ebay for "5V One Channel Relay Module Arduino")
1. Two red/green common anode LEDs
1. One pushbutton.
1. Some CAT5 network cable to hook up the sensor to the micro controller.
1. (optional) One DCDC buck converter to convert garage door opener's 24V to a more palatable 5V for the electronics.

