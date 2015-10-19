# mageboot
A bootloader for use with the [MAGE 2 PIU] (https://github.com/Jacob-Dixon/MAGE2/wiki/PIU) and [MCU Certification] (https://github.com/Jacob-Dixon/MAGE2/wiki/Microcontroller_Cert) at Oklahoma State University.

## Features
* Compatible with ATmega2560 and ATmega328P
* Boots into the application quickly
* MAGE 2 PIU only
  * Under 1k of flash
  * OTA upgrade support
* MCU Certification Board only
  * Under 512 bytes of flash

## Installation
mageboot must be flashed using [avrdude] (http://savannah.nongnu.org/projects/avrdude) and a compatible ICSP programmer. See [avrdude's documentation] (http://www.nongnu.org/avrdude/user-manual/avrdude.html) and [this tutorial] (http://www.ladyada.net/learn/avr/avrdude.html) for more information.

After mageboot has been flashed, programs can be uploaded using [mageload] (https://github.com/cjvaughter/mageload).

#### Fuse Settings
MAGE 2 PIU:

    Low: 0xFF High: 0xDE Extended: 0xFD
MCU Certification Board:

    Low: 0xFF High: 0xDE Extended: 0x05
See this [fuse calculator] (http://www.engbedded.com/fusecalc/) for more information.
