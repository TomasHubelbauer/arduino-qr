#include <LedControl.h> // https://github.com/wayoda/LedControl
#include <qrcode.h> // https://github.com/ricmoo/QRCode
#include <SoftwareSerial.h> // https://www.arduino.cc/en/Reference/softwareSerial

// TODO:
// - Add commands to allow setting QR code level and display scale (bounded by `size`)
// - Add an option to auto-detect QR code level and display scale (bounded by `size`)
// - Add an option to not auto-capitalize text but switch to binary mode instead

// DIN, CLK, CS
LedControl ledControls[] = {
  LedControl(2, 3, 4, 3),
  LedControl(5, 6, 7, 3),
  LedControl(11, 12, 13, 3),
};

QRCode qrcode;

// Note that not all pins are equal for software serial, these two work
SoftwareSerial serialBluetooth(10, 9);

int size = 3;
int side = size * 8;

void setup() {
  for (int index = 0; index < size; index++) {
    ledControls[index].shutdown(0, false);
    ledControls[index].shutdown(1, false);
    ledControls[index].shutdown(2, false);

    ledControls[index].setIntensity(0, 4);
    ledControls[index].setIntensity(1, 4);
    ledControls[index].setIntensity(2, 4);
  }

  Serial.begin(9600);
  serialBluetooth.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT);
  qr("HUBELBAUER.NET");
}

void loop() {
  // Echo messages from the Bluetooth serial module to normal serial and show them as QR code
  if (serialBluetooth.available()) {
    String text = serialBluetooth.readString();

    // Keep only the first line, the BLExAR app sometimes sends extra junk at the end?
    text = text.substring(0, text.indexOf("\n"));

    // Raise the casing as QR code can only encode lower case letters in binary mode not ASCII mode
    text.toUpperCase();

    // Print the received text to the serial monitor in Arduino IDE for debugging
    Serial.println(text);

    // Display the received text as a QR code on the dot matrix display grid
    qr(text.c_str());
  }

  // Echo messages from serial to the Bluetooth serial module to test communication in Arduino IDE
  if (Serial.available()) {
    serialBluetooth.write(Serial.read());
  }

  // Blink the internal LED to indicate liveness
  digitalWrite(LED_BUILTIN, (millis() / 1000) % 2 == 0 ? HIGH : LOW);
}

void qr(const char* text) {
  int version = 1;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  qrcode_initText(&qrcode, qrcodeData, version, ECC_LOW, text);

  int scale = 1;
  int shift = (side - (qrcode.size * scale)) / 2;
  for (int y = shift; y < side; y++) {
    for (int x = shift; x < side; x++) {
      int address = x / 8;
      int row = 7 - (y % 8);
      int column = 7 - (x % 8);
      bool state = false; // Turn out of bounds dots off to make sure they don't stay on
      if (x >= shift && x < shift + (qrcode.size * scale) && y >= shift && y < shift + (qrcode.size * scale)) {
        state = qrcode_getModule(&qrcode, (x - shift) / scale, (y - shift) / scale);
      }

      int index = y / 8;
      ledControls[index].setLed(address, row, column, state);
    }
  }
}
