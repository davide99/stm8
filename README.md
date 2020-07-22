# STM8s Open peripheral library

Work in progress...  
Contact me [@davide99](https://telegram.me/davide99)

## Requirements

1. Linux
1. git
1. Python >= 3.5
1. colorama for python 3 (use pip)
1. [SDCC](http://sdcc.sourceforge.net/snap.php)
1. [stm8flash](https://github.com/vdudouyt/stm8flash)

## Quick start

* `$ git clone https://github.com/davide99/stm8.git`
* `$ cd stm8`
* `$ sudo chmod +x make.py`
* `$ ./make.py flash`
* Done

## ST-Link Pinout

ST-Link V2 clones usually come in two flavours, the ones with the metal enclosing and the ones by Baite in green plastic
enclosing. They are both perfectly fine, despite the second one support also the JTAG protocol (besides SWIM and SWD)

![pinout_stlink](misc/pinout_stlink.png)

### Baite version

1. T_JRST
1. 3V3
1. 5V
1. T_JTCK/T_SWCLK
1. SWIM
1. T_JTMS/T_SWDIO
1. GND
1. T_JTDO
1. SWIM_RST
1. T_JTDI

### Plastic version

1. RST
1. SWDIO
1. GND
1. GND
1. SWIM
1. SWCLK
1. 3V3
1. 3V3
1. 5V
1. 5V

### MCU wiring

| Signal        | Baite ST-Link | Metal ST-Link |
|:-------------:|:-------------:|:-------------:|
| 3V3           | 2             | 7             |
| SWIM          | 5             | 5             |
| GND           | 7             | 3             |
| NRST          | 9             | 1             |

## CLion support
This repository is CLion-ready. After importing the repository the `Build & Flash` target is available. Just open the
main file you want to flash and hit the `►` button.