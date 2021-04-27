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

## To-Do

### Try to hook them up together and to an Arduino

Try to hook one of the LED matrix displays up to an Arduino and successfully
drive it.

There are 5 points on the board that comes with the display. Two of them are for
power - VCC (+5 V) and GND (ground). The remaining ones are for SPI. DIN is data
input, CS is an SPI chip select connection and CLK is the clock connection. The
power pins are easily taken care of so the only remaining this is to connect the
SPI pins to available Arduino pins.

Arduino has a library called [LEDControl](https://www.arduino.cc/reference/en/libraries/ledcontrol/),
which among other things can also drive these 8×8 LED matrices based around the
MAX7219 display driver.

Sample code to drive a single display should look more or less like this:
(See the [LEDControl documentation](http://wayoda.github.io/LedControl/pages/software)).

```ino
#include <LedControl.h>

// These can be any of the Arduino digital pins
int DIN = 12;
int CLK = 11;
int CS = 10;

// The 4th argument is the number of displays in the chain (1-8)
// Note that LedControl will default the display to a blank state (cleared),
// the `clearDisplay` method can be used to clear the display later on.
LedControl ledControl = LedControl(DIN, CLK, CS, 1);

void setup() {
  // Leave power-saving mode (MAX72XX is in shutdown mode on Arduino power-up)
  // Note that in the shutdown mode, the display remains lit with the last data
  // sent before entering the power-saving mode. New data can be sent, but will
  // only be displayed once the power-saving mode has been exited.
  // TODO: See if I can remove this here and just keep it in the loop/interrupt
  ledControl.shutdown(0, false);
  
  // Set the intensity of the display (0-15, even 0 is still slightly lit)
  ledControl.setIntensity(0, 8);
}

// TODO: See if I can move shutdown leave after data set to keep it in it longer
void loop() {
  // Exit power-saving mode
  ledControl.shutdown(0, false);
  
  // Light all of the LEDs in the matrix display on
  for (int row = 0; row < 8; row++) {
    for (int column = 0; column < 8; column++) {
      ledControl.setLed(0, row, column, true);
    }
  }
  
  // Enter power-saving mode (retain currently displayed data)
  ledControl.shutdown(0, true);
  
  // Wait before re-drawing the matrix display
  // TODO: Replace this with an interrupt handler for a button press
  delay(1000);
}
```

Useful functions from [the documentation](http://wayoda.github.io/LedControl/pages/software):

- `void clearDisplay(int *addr)`
- `void setLed(int addr, int row, int col, boolean state)`
- `void setRow(int addr, int row, byte value)`
- `void setColumn(int addr, int col, byte value)`

The `value` of `setRow` and `setColumn` is a binary number, e.g.: `B10001000`.

[LEDControl matrix display example](https://github.com/wayoda/LedControl/blob/master/examples/LCDemoMatrix/LCDemoMatrix.ino)

Driving multiple matrices is done by changing the 4th argument of `LedControl`.
A single instance of `LedControl` supports up to 8 displays. To drive more than
8 displays, multiple instances of `LedControl` need to be used. A single board
with 13 digital pins can support up to 4 chains of 8 displays, so 32 displays in
total.

### Build an enclosure to hold the 4×4 matrix of the displays together and chain

Once a single displays is proven to work, try chaining them and driving them as
a single big display.

### Generate and display a QR code

https://www.arduino.cc/reference/en/libraries/qrcode/
