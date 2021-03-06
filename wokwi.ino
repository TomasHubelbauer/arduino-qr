#include <LedControl.h> // https://github.com/wayoda/LedControl
#include <qrcode.h> // https://github.com/ricmoo/QRCode

int size = 6;
int side = size * 8;
QRCode qrcode;

// DIN, CLK, CS
LedControl ledControls[] = {
  LedControl(0, 1, 2, size),
  LedControl(3, 4, 5, size),
  LedControl(6, 7, 8, size),
  LedControl(9, 10, 11, size),
  LedControl(A0, A1, A2, size),
  LedControl(A3, A4, A5, size),
};

int main() {
  int version = 1;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  qrcode_initText(&qrcode, qrcodeData, version, ECC_LOW, "HUBELBAUER.NET");

  int scale = 2;
  int shift = (side - (qrcode.size * scale)) / 2;
  for (int y = shift; y < side; y++) {
    for (int x = shift; x < side; x++) {
      int index = y / 8;
      int address = size - 1 - (x / 8);
      int row = y % 8;
      int column = 7 - (x % 8);

      // Turn out of bounds dots off to make sure they don't stay on
      bool state = false;
      if (x >= shift && x < shift + (qrcode.size * scale) && y >= shift && y < shift + (qrcode.size * scale)) {
        state = qrcode_getModule(&qrcode, (x - shift) / scale, (y - shift) / scale);
      }

      ledControls[index].setLed(address, row, column, state);
    }
  }
}
