#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif


#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const uint8_t *)(addr))
#endif


#define FONT_START 0
#define FONT_END   1

struct FontDescriptor
{
  uint8_t width;    // width in bits
  uint8_t height;   // char height in bits
  uint16_t offset;  // offset of char into char array
};

// Font references - add pair of references for each new font

#ifdef font_gausshauss_22
#include "font_gausshauss_22px.h"
#endif

#ifdef font_marlboro_23
#include "font_marlboro_23px.h"
#endif

#ifdef font_marlboro_46
#include "font_marlboro_46px.h"
#endif

#ifdef font_nearly_24
#include "font_nearly_24px.h"
#endif

#ifdef font_parsons_46
#include "font_parsons_46px.h"
#endif

#ifdef font_republica_22
#include "font_republica_22px.h"
#endif

#ifdef font_tahoma_24
#include "font_tahoma_24px.h"
#endif

#ifdef font_titania_24
#include "font_titania_24px.h"
#endif

#ifdef font_titania_46
#include "font_titania_46px.h"
#endif


#define agswap(a, b) { int16_t t = a; a = b; b = t; }

class Adafruit_GFX : public Print {

 public:

  void setFont(uint8_t f) {
    font = f;
    fontKern = 1;
    switch(font) {

      // Gausshauss
      #ifdef font_gausshauss_22
      case font_gausshauss_22:
        fontData = gausshauss_16ptBitmaps;
        fontDesc = gausshauss_16ptDescriptors;
        break;
      #endif

      // Marlboro
      #ifdef font_marlboro_23
      case font_marlboro_23:
        fontData = marlboro_19ptBitmaps;
        fontDesc = marlboro_19ptDescriptors;
        break;
      #endif
      #ifdef font_marlboro_46
      case font_marlboro_46:
        fontData = marlboro_36ptBitmaps;
        fontDesc = marlboro_36ptDescriptors;
        break;
      #endif

      // Nearly Dignified Condensed
      #ifdef font_nearly_24
      case font_nearly_24:
        fontData = nearlyDignifiedCondensed_16ptBitmaps;
        fontDesc = nearlyDignifiedCondensed_16ptDescriptors;
        break;
      #endif

      // Parsons
      #ifdef font_parsons_46
      case font_parsons_46:
        fontData = parsons_34ptBitmaps;
        fontDesc = parsons_34ptDescriptors;
        break;
      #endif

      // Republica Minor
      #ifdef font_republica_22
      case font_republica_22:
        fontData = republicaMinor_13ptBitmaps;
        fontDesc = republicaMinor_13ptDescriptors;
        break;
      #endif

      // Tahoma
      #ifdef font_tahoma_24
      case font_tahoma_24:
        fontData = tahoma_17ptBitmaps;
        fontDesc = tahoma_17ptDescriptors;
        break;
      #endif

      // Titania
      #ifdef font_titania_24
      case font_titania_24:
        fontData = titania_17ptBitmaps;
        fontDesc = titania_17ptDescriptors;
        break;
      #endif
      #ifdef font_titania_46
      case font_titania_46:
        fontData = titania_33ptBitmaps;
        fontDesc = titania_33ptDescriptors;
        break;
      #endif

      // Arial
      #ifdef font_arial_8
      case font_arial_8:
      default:
        fontData = arial_8ptBitmaps;
        fontDesc = arial_8ptDescriptors;
        fontKern = 1;
        break;
      #endif
    }

    fontStart = pgm_read_byte(fontData+FONT_START);
    fontEnd = pgm_read_byte(fontData+FONT_END);
  }

  Adafruit_GFX(int16_t w, int16_t h):
    WIDTH(w), HEIGHT(h)
  {
    _width    = WIDTH;
    _height   = HEIGHT;
    rotation  = 0;
    cursor_y  = cursor_x = 0;
    textsize  = 1;
    textcolor = textbgcolor = 0x0000;
    wrap      = false;
    // setFont(lucidaSansUnicode_24);
   }

  // This MUST be defined by the subclass:
  virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;


  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
      agswap(x0, y0);
      agswap(x1, y1);
    }

    if (x0 > x1) {
      agswap(x0, x1);
      agswap(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
      ystep = 1;
    } else {
      ystep = -1;
    }

    for (; x0<=x1; x0++) {
      if (steep) {
        drawPixel(y0, x0, color);
      } else {
        drawPixel(x0, y0, color);
      }
      err -= dy;
      if (err < 0) {
        y0 += ystep;
        err += dx;
      }
    }
  }

  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    drawLine(x, y, x, y+h-1, color);
  }

  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // Update in subclasses if desired!
    drawLine(x, y, x+w-1, y, color);
  }

  virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y+h-1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x+w-1, y, h, color);
  }

  virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    // Update in subclasses if desired!
    for (int16_t i=x; i<x+w; i++) {
      drawFastVLine(i, y, h, color);
    }
  }

  virtual void fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
  }

  virtual void invertDisplay(boolean i) {
    // Do nothing, must be subclassed if supported
  }

  virtual void drawFastChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
    // Update in subclasses if desired!
    drawChar(x,y,c,color,bg,size);
  }

  // These exist only with Adafruit_GFX (no subclass overrides)
  
  void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0  , y0+r, color);
    drawPixel(x0  , y0-r, color);
    drawPixel(x0+r, y0  , color);
    drawPixel(x0-r, y0  , color);

    while (x<y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x;

      drawPixel(x0 + x, y0 + y, color);
      drawPixel(x0 - x, y0 + y, color);
      drawPixel(x0 + x, y0 - y, color);
      drawPixel(x0 - x, y0 - y, color);
      drawPixel(x0 + y, y0 + x, color);
      drawPixel(x0 - y, y0 + x, color);
      drawPixel(x0 + y, y0 - x, color);
      drawPixel(x0 - y, y0 - x, color);
    }
  }

  void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f     += ddF_y;
      }
      x++;
      ddF_x += 2;
      f     += ddF_x;
      if (cornername & 0x4) {
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 + y, y0 + x, color);
      }
      if (cornername & 0x2) {
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 + y, y0 - x, color);
      }
      if (cornername & 0x8) {
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 - x, y0 + y, color);
      }
      if (cornername & 0x1) {
        drawPixel(x0 - y, y0 - x, color);
        drawPixel(x0 - x, y0 - y, color);
      }
    }
  }

  void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    drawFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
  }

  void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
      int16_t delta, uint16_t color) {
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) {
      if (f >= 0) {
        y--;
        ddF_y += 2;
        f     += ddF_y;
      }
      x++;
      ddF_x += 2;
      f     += ddF_x;

      if (cornername & 0x1) {
        drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
        drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
      }
      if (cornername & 0x2) {
        drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
        drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
      }
    }
  }

  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
  }

  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
      int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
      agswap(y0, y1); agswap(x0, x1);
    }
    if (y1 > y2) {
      agswap(y2, y1); agswap(x2, x1);
    }
    if (y0 > y1) {
      agswap(y0, y1); agswap(x0, x1);
    }

    if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
      a = b = x0;
      if(x1 < a)      a = x1;
      else if(x1 > b) b = x1;
      if(x2 < a)      a = x2;
      else if(x2 > b) b = x2;
      drawFastHLine(a, y0, b-a+1, color);
      return;
    }

    int16_t
      dx01 = x1 - x0,
      dy01 = y1 - y0,
      dx02 = x2 - x0,
      dy02 = y2 - y0,
      dx12 = x2 - x1,
      dy12 = y2 - y1,
      sa   = 0,
      sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) {
      a   = x0 + sa / dy01;
      b   = x0 + sb / dy02;
      sa += dx01;
      sb += dx02;
      /* longhand:
      a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
      */
      if(a > b) agswap(a,b);
      drawFastHLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) {
      a   = x1 + sa / dy12;
      b   = x0 + sb / dy02;
      sa += dx12;
      sb += dx02;
      /* longhand:
      a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
      b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
      */
      if(a > b) agswap(a,b);
      drawFastHLine(a, y, b-a+1, color);
    }
  }

  void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
      int16_t r, uint16_t color) {
    // smarter version
    drawFastHLine(x+r  , y    , w-2*r, color); // Top
    drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawFastVLine(x    , y+r  , h-2*r, color); // Left
    drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
  }

  void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
      int16_t r, uint16_t color) {
    // smarter version
    fillRect(x+r, y, w-2*r, h, color);

    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
  }

  void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
      int16_t w, int16_t h, uint16_t color) {
    int16_t i, j, byteWidth = (w + 7) / 8;

    for(j=0; j<h; j++) {
      for(i=0; i<w; i++ ) {
        if(pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
    drawPixel(x+i, y+j, color);
        }
      }
    }
  }

  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
      uint16_t bg, uint8_t size) {
    if (c < fontStart || c > fontEnd) {
      c = 0;
    }
    else {
      c -= fontStart;
    }

    if((x >= _width)            || // Clip right
       (y >= _height)           || // Clip bottom
       ((x + (fontDesc[c].width * size) - 1) < 0) || // Clip left
       ((y + (fontDesc[c].height * size) - 1) < 0))   // Clip top
      return;

    uint8_t bitCount=0;
      uint16_t fontIndex = fontDesc[c].offset + 2; //((fontDesc + c)->offset) + 2;

    for (int8_t i=0; i<fontDesc[c].height; i++ ) {  // i<fontHeight
      uint8_t line;
      for (int8_t j = 0; j<fontDesc[c].width; j++) {      //j<fontWidth
        if (bitCount++%8 == 0) {
          line = pgm_read_byte(fontData+fontIndex++);
        }
        if (line & 0x80) {
          if (size == 1) {// default sizeFast
            drawPixel(x+j, y+i, color);
            }
          else {  // big size
            fillRect(x+(j*size), y+(i*size), size, size, color);
          }
        } else if (bg != color) {
          if (size == 1) // default size
            drawPixel(x+j, y+i, bg);
          else {  // big size
            fillRect(x+j*size, y+i*size, size, size, bg);
          }
        }
        line <<= 1;
      }
      bitCount = 0;
    }
  }

  void setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }

  void setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
  }

  void setTextColor(uint16_t c, uint16_t bg) {
    textcolor   = c;
    textbgcolor = bg;
  }

  void setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
  }

  void setTextWrap(boolean w) {
    wrap = w;
  }

  void setRotation(uint8_t r) {
    rotation = (r & 3);
    switch(rotation) {
     case 0:
     case 2:
      _width  = WIDTH;
      _height = HEIGHT;
      break;
     case 1:
     case 3:
      _width  = HEIGHT;
      _height = WIDTH;
      break;
    }
  }

  virtual size_t write(uint8_t c) {
    if (c == '\n') {
      cursor_y += textsize*fontDesc[0].height;  //all chars are same height so use height of space char
      cursor_x  = 0;
    } else if (c == '\r') {
      // skip em
    } else {
      drawFastChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
      uint16_t w = fontDesc[c-fontStart].width;
      uint16_t h = fontDesc[c-fontStart].height;
      if (fontKern > 0 && textcolor != textbgcolor) {
        fillRect(cursor_x+w*textsize,cursor_y,fontKern*textsize,h*textsize,textbgcolor);
      }
      cursor_x += textsize*(w+fontKern);
      if (wrap && (cursor_x > (_width - textsize*w))) {
        cursor_y += textsize*h;
        cursor_x = 0;
      }
    }
    return 1;
  }


  int16_t height(void) {
    return _height;
  }

  int16_t width(void) {
    return _width; 
  }

  uint8_t getRotation(void) {
    return rotation;
  }

 protected:
  const int16_t
    WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
  int16_t
    _width, _height, // Display w/h as modified by current rotation
    cursor_x, cursor_y;
  uint16_t
    textcolor, textbgcolor;
  uint8_t
    textsize,
    rotation,
    font,
    fontStart,
    fontEnd;
  int8_t
    fontKern;
  const uint8_t* fontData;
  const FontDescriptor* fontDesc;
  boolean
    wrap; // If set, 'wrap' text at right edge of display
};

#endif // _ADAFRUIT_GFX_H
