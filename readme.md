# Arduino QR Code

I'm wondering whether it would be possible to build a QR code display using 8×8
LED matrix displays and an Arduino or a Pi Pico.

According to Wikipedia [QR Code Storage](https://en.wikipedia.org/wiki/QR_code#Storage),
the various versions require these sorts of sizes:

| QR Version | QR Size | 8×8 LED Matrix Display Count | Utilization           |
|------------|---------|------------------------------|-----------------------|
| 1          | 21×21   | 9 (3×3 - 24×24)              | 77 % (441/576 dots)   |
| 2          | 25×25   | 16 (4×4 - 32×32)             | 61 % (625/1024 dots)  |
| 3          | 29×29   | 16 (4×4 - 32×32)             | 82 % (841/1024 dots)  |
| 4          | 33×33   | 25 (5×5 - 40×40)             | 90 % (1089/1200 dots) |

The 8×8 LED matrix displays can be bought from:

- [Czech Republic: HWKitchen](https://www.hwkitchen.cz/8x8-led-matrix-cervena-s-max7219-diy-kit)
- [China: AliExpress](https://www.aliexpress.com/item/32849877252.html)

The displays seem to be chainable up to 8 displays in a chain and each chain of
up to 8 displays requires 3 SPI pins. [Arduinos generally have 13 digital pins](https://www.arduino.cc/en/reference/board)
so a single board with no components other than the displays can drive up to
32 displays (four triples of each chains' SPI pins). The biggest square that can
be made this way and driven using a single Arduino is a 5×5 LED matrix display.
6×6 would be 36 displays total, exceeding the maximum handle-able by the Arduino
(32).

## Wokwi Simulation

`qrcode.ino`
```ino
#include <LedControl.h>
#include "qrcode.h"

LedControl ledControl0 = LedControl(/* DIN */ 0, /* CLK */ 1, /* CS */ 2, /* Count */ 4);
LedControl ledControl1 = LedControl(/* DIN */ 3, /* CLK */ 4, /* CS */ 5, /* Count */ 4);
LedControl ledControl2 = LedControl(/* DIN */ 6, /* CLK */ 7, /* CS */ 8, /* Count */ 4);
LedControl ledControl3 = LedControl(/* DIN */ 9, /* CLK */ 10, /* CS */ 11, /* Count */ 4);
QRCode qrcode;

void setup() {
  ledControl0.setIntensity(0, 15);

  // Allocate array large enough to hold a version 3 QR code (29*29)
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, "HELLO WORLD");

  drawQrCode();
  //drawPixel(31, 15);
}

void loop() {
  // TODO: Replace this with ArduinoLowPower.h and its power-down mode:
  // LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  delay(2147483647);
}

void drawPixel(int x, int y) {
  for (int index = 0; index < 2; index++) {
    LedControl ledControl = index == 0 ? ledControl0 : ledControl1;
    if (index == y / 8) {
      ledControl.shutdown(0, false);

      ledControl.clearDisplay(0);
      ledControl.clearDisplay(1);
      ledControl.clearDisplay(2);
      ledControl.clearDisplay(3);

      int address = 3 - (x / 8);
      int row = y % 8;
      int column = 7 - (x % 8);
      ledControl.setLed(address, row, column, true);

      ledControl.shutdown(0, true);
    }
  }
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

      // TODO: Figure out how to make `LedControl ledControl = index == 0 ? ledControl0 : ledControl1` work
      // Right now, when I do that, the displays behave very strangely
      int index = y / 8;
      switch (index) {
        case 0: {
          ledControl0.shutdown(address, false);
          ledControl0.setLed(address, row, column, state);
          ledControl0.shutdown(address, true);
          break;
        }
        case 1: {
          ledControl1.shutdown(address, false);
          ledControl1.setLed(address, row, column, state);
          ledControl1.shutdown(address, true);
          break;
        }
        case 2: {
          ledControl2.shutdown(address, false);
          ledControl2.setLed(address, row, column, state);
          ledControl2.shutdown(address, true);
          break;
        }
        case 3: {
          ledControl3.shutdown(address, false);
          ledControl3.setLed(address, row, column, state);
          ledControl3.shutdown(address, true);
          break;
        }
      }
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

Driving multiple matrices is done by changing the 4th argument of `LedControl`.
A single instance of `LedControl` supports up to 8 displays. To drive more than
8 displays, multiple instances of `LedControl` need to be used. A single board
with 13 digital pins can support up to 4 chains of 8 displays, so 32 displays in
total.

[Wokwi project](https://wokwi.com/arduino/projects/297148152803230218)

## To-Do

### Wire up the Wokwi simulation using a real Arduino and MAX7219 display grid

[Wokwi project](https://wokwi.com/arduino/projects/297148152803230218)

### Build an enclosure to hold the 4×4 matrix of the displays together

3D-print an enclosure to hold the displays together.
