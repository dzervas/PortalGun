# PortalGun

Code to control a Portal Gun (From Rick and Morty)
Χρόνια πολλά Γιάνναρε!!!

## Pin Definitions

If you deviate from the following definitions, you will have to change the firmware to account for that.

| LED Display | Arduino Nano Pin |
|------------------------|-------|
|          CLK           |   7   |
|         LATCH          |   6   |
|          DATA          |   5   |

| Rotary Encoder | Arduino Nano Pin |
|---------|-------|
|    A    |   A1  |
|    B    |   A0  |
|   GND   |  GND  |
| Button  |   A2  |

| LED | Arduino Nano Pin |
|--------------|---------|
| Top Bulb     |    12   |
| Front Right  |    8    |
| Front Center |    4    |
| Front Left   |    2    |

## Button Behavior

The rotary encoder has a click button, and we can detect a single click, a double click, and a hold.

- Single Click : Wakes the Arduino from low power mode and plays an animation
- Double Click : Reset to dimension C137
- Hold : Turn off LEDs and put the Arduino into a low power mode.

## Installing Firmware

I chose to go with [PlatformIO](https://platformio.org) instead of the crappy
Arduino IDE. Has the whole arduino build system AND library management solved
with ultra minimal setup. To compile and upload the firmware:

```bash
pip install --user -U platformio
pio run --target upload [--upload-port /dev/ttyUSB0]
# To view serial messages:
pio device monitor [-p /dev/ttyUSB0]
```
