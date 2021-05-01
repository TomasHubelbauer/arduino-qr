# Arduino QR Code

An Arduino QR code display using a 4×4 matrix of 8×8 LED matrix displays using
the MAX7219 display driver. [AliExpress](https://www.aliexpress.com/item/32849877252.html)

![](wokwi.png)

The displays are chainable up to 8 displays in one chain. Multiple chains need
to be used to drive more displays. Arduino with 13 digital pins can support up
to 4 chains (3 SPI pins per chain: DIN, CLK and CS). That gives us the maximum
display size of 32×32 dots (4×4 displays to keep the compound display square).
This allows displaying QR codes of versions 1, 2 and 3:

[Wikipedia: QR Code Storage](https://en.wikipedia.org/wiki/QR_code#Storage):

| QR Version | QR Size | 8×8 LED Matrix Display Count | Utilization           |
|------------|---------|------------------------------|-----------------------|
| 1          | 21×21   | 9 (3×3 - 24×24)              | 77 % (441/576 dots)   |
| 2          | 25×25   | 16 (4×4 - 32×32)             | 61 % (625/1024 dots)  |
| 3          | 29×29   | 16 (4×4 - 32×32)             | 82 % (841/1024 dots)  |

[QRCode.com Information Capacity](https://www.qrcode.com/en/about/version.html)

| QR Version | Error Correction | Numeric Capacity | Alphanumeric Capacity |
|------------|------------------|------------------|-----------------------|
| 1          | Low              | 41               | 25                    |
| 1          | Medium           | 34               | 20                    |
| 1          | Quartile         | 27               | 16                    |
| 1          | High             | 17               | 10                    |
| 2          | Low              | 77               | 47                    |
| 2          | Medium           | 63               | 38                    |
| 2          | Quartile         | 48               | 29                    |
| 2          | High             | 34               | 20                    |
| 3          | Low              | 127              | 77                    |
| 3          | Medium           | 101              | 61                    |
| 3          | Quartile         | 77               | 47                    |
| 3          | High             | 58               | 35                    |

## [Wokwi Simulation](https://wokwi.com/arduino/projects/297148152803230218)

`qrcode.ino`
```ino
#include <LedControl.h>
#include "qrcode.h" // https://github.com/ricmoo/QRCode

QRCode qrcode;
LedControl ledControls[] = {
  LedControl(/* DIN */ 0, /* CLK */ 1, /* CS */ 2, /* Count */ 4),
  LedControl(/* DIN */ 3, /* CLK */ 4, /* CS */ 5, /* Count */ 4),
  LedControl(/* DIN */ 6, /* CLK */ 7, /* CS */ 8, /* Count */ 4),
  LedControl(/* DIN */ 9, /* CLK */ 10, /* CS */ 11, /* Count */ 4),
};

void setup() {
  ledControls[0].setIntensity(0, 15);
  ledControls[1].setIntensity(0, 15);
  ledControls[2].setIntensity(0, 15);
  ledControls[3].setIntensity(0, 15);

  // Allocate array large enough to hold a version 3 QR code (29*29)
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, "HELLO WORLD");

  drawQrCode();
}

void loop() {
  // TODO: Replace this with ArduinoLowPower.h and its power-down mode:
  // LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  delay(2147483647);
}

void drawQrCode() {
  // TODO: Adjust the code for this to work and center the QR code
  int pad = (32 - qrcode.size) / 2;
  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 32; x++) {
      int address = 3 - (x / 8);
      int row = y % 8;
      int column = 7 - (x % 8);
      bool state = qrcode_getModule(&qrcode, x, y);
      int index = y / 8;
      ledControls[index].shutdown(address, false);
      ledControls[index].setLed(address, row, column, state);
      ledControls[index].shutdown(address, true);
    }
  }
}
```

`diagram.json`
```json
{
  "version": 1,
  "author": "Tomas Hubelbauer",
  "editor": "wokwi",
  "serialMonitor": {
    "display": "never"
  },
  "parts": [
    {
      "type": "wokwi-arduino-uno",
      "id": "uno",
      "top": 30,
      "left": 0,
      "rotate": 90,
      "hide": false,
      "attrs": {}
    },
    {
      "type": "wokwi-max7219-matrix",
      "id": "m1",
      "top": 0,
      "left": 300,
      "rotate": 0,
      "hide": false,
      "attrs": { "chain": "4" }
    },
    {
      "type": "wokwi-max7219-matrix",
      "id": "m2",
      "top": 75,
      "left": 300,
      "rotate": 0,
      "hide": false,
      "attrs": { "chain": "4" }
    },
    {
      "type": "wokwi-max7219-matrix",
      "id": "m3",
      "top": 150,
      "left": 300,
      "rotate": 0,
      "hide": false,
      "attrs": { "chain": "4" }
    },
    {
      "type": "wokwi-max7219-matrix",
      "id": "m4",
      "top": 225,
      "left": 300,
      "rotate": 0,
      "hide": false,
      "attrs": { "chain": "4" }
    }
  ],
  "connections": [
    [ "uno:GND.1", "m1:GND", "black", [ "v-12", "*", "h-16" ] ],
    [ "uno:5V", "m1:V+", "red", [ "v35", "h-10", "*", "h-40" ] ],
    [ "uno:0", "m1:DIN", "orange", [ "v-20", "*", "h-8" ] ],
    [ "uno:1", "m1:CLK", "blue", [ "v-16", "*", "h-12" ] ],
    [ "uno:2", "m1:CS", "green", [ "v-24", "*", "h-4" ] ],

    [ "uno:GND.1", "m2:GND", "black", [ "v-12", "*", "h-16" ] ],
    [ "uno:5V", "m2:V+", "red", [ "v35", "h-10", "*", "h-40" ] ],
    [ "uno:3", "m2:DIN", "orange", [ "v-20", "*", "h-8" ] ],
    [ "uno:4", "m2:CLK", "blue", [ "v-16", "*", "h-12" ] ],
    [ "uno:5", "m2:CS", "green", [ "v-24", "*", "h-4" ] ],

    [ "uno:GND.1", "m3:GND", "black", [ "v-12", "*", "h-16" ] ],
    [ "uno:5V", "m3:V+", "red", [ "v35", "h-10", "*", "h-40" ] ],
    [ "uno:6", "m3:DIN", "orange", [ "v-20", "*", "h-8" ] ],
    [ "uno:7", "m3:CLK", "blue", [ "v-16", "*", "h-12" ] ],
    [ "uno:8", "m3:CS", "green", [ "v-24", "*", "h-4" ] ],

    [ "uno:GND.1", "m4:GND", "black", [ "v-12", "*", "h-16" ] ],
    [ "uno:5V", "m4:V+", "red", [ "v35", "h-10", "*", "h-40" ] ],
    [ "uno:9", "m4:DIN", "orange", [ "v-20", "*", "h-8" ] ],
    [ "uno:10", "m4:CLK", "blue", [ "v-16", "*", "h-12" ] ],
    [ "uno:11", "m4:CS", "green", [ "v-24", "*", "h-4" ] ]
  ]
}
```

`qrcode.c` and `qrcode.h` from [ricmoo/QRCode](https://github.com/ricmoo/QRCode)
also need to be added to the project.

[LEDControl](https://www.arduino.cc/reference/en/libraries/ledcontrol) will work
out of the box.

[LEDControl documentation](http://wayoda.github.io/LedControl/pages/software)

## To-Do

### Wire up the Wokwi simulation using a real Arduino and MAX7219 display grid

### 3D-print an enclosure to hold the 4×4 matrix of the displays together

### Experiment with normal and inverted QR display to see what reads better

### Order and try white dot-matrix displays to see if they scan better than red

### Add a push button / SD reader / HTTP server or something to update the code

It would be useful to allow updating the code once flashed. The most flexible
way would be to have a built-in HTTP server. The initial code shown would be a
URL to the server and accessing that server, once could set new values of the
QR code. A push button would be used to reset to the server URL code. Or use an
SD card reader which reads a value of a text file.

### See if I can make this work on the Raspberry Pi Pico using Wokwi Pico sim

The Pico can mount itself as a mass storage device, so the text file QR content
source idea would be that much easier.

### Verify LEDControl can truly only drive 8 dot matrix displays in a chain

Test this on actual hardware. Check other libraries too, to find whichever is
able to control the most displays.

### Test the method of using analog pins as digital and recalculate maximum

https://arduino.stackexchange.com/q/117/23747

This talks about using analog pins as digital, test if LEDControl can be
configured to use those and recalculate the new maximum size of a square
compound display if we were to utilize both the digital and the analog pins.

It looks like the total number of pins would be 19, which means 19/3 (6) chains
of dot matrix displays. Potentially, then, the new maximum should be a six-sided
compound display, so 48×48 dots? This would support version 4 QR codes or almost
allow a double-thickness version 2 QR code for better scanning support.

### Look into using a shift register shield to extend the amount of pins

This would enable QR codes of higher versions and could also enable doubling up
the QR "thickness" for more reliable scanning (if that would help…).

https://nootropicdesign.com/ez-expander

If I'm counting right, this would give a total of 44 pins to use, if we included
the analog pins, too. 44 pins means 44/3 (14) chains. If the chains are stuck at
the length of 8 dot matrix displays, this would at least give us an 8×8 compound
display (64×64 dots). If they worked past 8, this would give us 14×14 compound
display (112×112 dots). 64×64 would support QR version 10 or doubled up version
version 3. 112×112 would support doubled up version 10 and just slightly miss
the threshold for version 25 (117×117). A doubled up version 10 would be super
sweet if it improved the scanning quality. It seems to support 174 alphanumeric
characters with high error correction.
