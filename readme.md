# Arduino QR Code

I'm wondering whether it would be possible to build a QR code display using 8x8
LED matrices and an Arduino.

According to Wikipedia, the various versions require these sorts of sizes:
https://en.wikipedia.org/wiki/QR_code#Storage

1. 21x21 would have to buy 3x3 matrices (24x24) to 77 % use (441/576 dots used)
2. 25x25 would have to buy 4x4 matrices (32x32) to 61 % use (625/1024 dots used)
3. 29x29 would have to buy 4x4 matrices (32x32) to 82 % use (841/1024 dots used)

The segment can be bought in the Czech Republic or from China:

https://www.hwkitchen.cz/8x8-led-matrix-cervena-s-max7219-diy-kit

It looks like these modules should be combinable and the amount of pins needed
on the Arduino remains constant not matter what the count of the matrices. This
should simplify things and make it so that no shift registers are required.

## To-Do

### Buy the matrices and try to hook them up together and to an Arduino

https://www.hwkitchen.cz/8x8-led-matrix-cervena-s-max7219-diy-kit

The link contains some information and links about the connection and setup.
