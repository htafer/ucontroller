Adafruit_mfGFX_Library
======================

Multifont version of Adafruit_GFX library, with fonts loaded as-needed. Supports several fonts, but only loads the ones you use into flash memory.

This library supports any compatible Adafruit_GFX display driver. The free TheDotFactory Windows program is recommended for created data compatible with the Adafruit_mfGFX library.

Please see the README.pdf files for instructions on creating and adding new fonts.

Fonts:
------

| Font Name  | Size | Include |
| ---------- | ---- | ------- |
| Arial      | 8pt  | #define font_arial_8 1 |
| Dejavu     | 24pt | #define font_dejavu_24 2 |
| Dejavu     | 48pt | #define font_dejavu_48 3 |
| Gausshauss | 22px | #define font_gausshauss_22 4 |
| Lucida     | 24pt | #define font_lucida_24 5 |
| Marlboro   | 23pt | #define font_marlboro_23 6 |
| Marlboro   | 46pt | #define font_marlboro_46 7 |
| Nearly     | 24px | #define font_nearly_24 8 |
| Parsons    | 46px | #define font_parsons_46 9 |
| Republica  | 22px | #define font_republica_22 10 |
| Tahoma     | 24px | #define font_tahoma_24 11 |
| Titania    | 24px | #define font_titania_24 12 |
| Titania    | 46px | #define font_titania_46 13 |


Example:
--------

```
// see Adafruit_mfGFX.h for a complete list of supported fonts.
// Note: Each defined font MUST have a unique ID. It doesn't matter what the
// integer ID is, as long as it's unique. For instance:
#define font_gausshauss_22   1
#define font_marlboro_48     2
#define font_republica_22    3

#include <Adafruit_mfGFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

void writeMedium(const char *str, uint8_t line) {
  display.setFont(font_gausshauss_22);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 16 + line * 24);

  int i = 0;
  while(i < 20 && str[i] != '\0') {
    display.write(str[i]);
    i++;
  }
}

void setup(void) {
    // initialize display, then...
    writeMedium("Hi!", 0);
}
```
