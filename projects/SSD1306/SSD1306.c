#include "board.h"
#include "bflb_gpio.h"
#include "bflb_i2c.h"
#include "glcdfont.c"
#include "gfxfont.h"
#include "SSD1306.h"

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

static struct bflb_device_s *i2c0;

struct bflb_i2c_msg_s msgs[1];

uint8_t write_data[7]; // Массив для работы с командами
uint8_t write_buff[32]; // Массив для работы с буфером

uint8_t *buffer;

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

//Инициализация дисплея
void SSD1306_init()
{
    //Инициализация I2C
    board_i2c0_gpio_init(); // SDA -> IO11, SCL -> IO14
    i2c0 = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(i2c0, 400000);

    // Размещение буфера в памяти
    buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8));
    
    //Запись последовательности в дисплей
    write_data[0] = 0x00;
    write_data[1] = 0xAE; //Set display OFF (sleep mode)
    write_data[2] = 0xD5; //Set Display Clock Divide Ratio / OSC Frequency
    write_data[3] = 0x80; //Display Clock Divide Ratio / OSC Frequency
    write_data[4] = 0xA8; //Set Multiplex Ratio

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 5;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0x3F; // Multiplex Ratio for 128x64 (64-1)

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 2;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0xD3; // Set Display Offset
    write_data[2] = 0x00; // Display Offset
    write_data[3] = 0x40; // Set Display Start Line
    write_data[4] = 0x8D; // Set Charge Pump

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 5;

    bflb_i2c_transfer(i2c0, msgs, 1);
  
    write_data[0] = 0x00;
    write_data[1] = 0x14; // Charge Pump (0x10 External, 0x14 Internal DC/DC)

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 2;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0x20; // Set memory adressing mode
    write_data[2] = 0x00; // Horizontal adressing mode
    write_data[3] = 0xA1; // Set Segment Re-Map
    write_data[4] = 0xC8; // Set Com Output Scan Direction

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 5;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0xDA; // Set COM Hardware Configuration
    write_data[2] = 0x12; // COM Hardware Configuration

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 3;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0x81; // Set Contrast
    write_data[2] = 0xCF; // Contrast

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 3;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0xD9; // Set Pre-Charge Period
    write_data[2] = 0xF1; // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 3;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0xDB; // Set VCOMH Deselect Level
    write_data[2] = 0x40; // VCOMH Deselect Level

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 3;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0xA4; // Set entire display
    write_data[2] = 0xA6; // Set display not inverted
    write_data[3] = 0x2E; // Deactivate scroll
    write_data[4] = 0xAF; // Set display ON from sleep mode

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 5;

    bflb_i2c_transfer(i2c0, msgs, 1);
    
}

// Очистка буфера дисплея
void SSD1306_clear()
{
    memset(buffer, 0x00, WIDTH * ((HEIGHT + 7) / 8));
}

// Отображение буфера дисплея
void SSD1306_display()
{
    write_data[0] = 0x00;
    write_data[1] = 0x22; // Page_addr command
    write_data[2] = 0x00; // Page start address
    write_data[3] = 0xFF; // Page end (not really, but works here)
    write_data[4] = 0x21; // Column address
    write_data[5] = 0x00; // Column start address (com = 0)

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 6;

    bflb_i2c_transfer(i2c0, msgs, 1);

    write_data[0] = 0x00;
    write_data[1] = 0x7F; // Column end address (com = 63)

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 2;

    bflb_i2c_transfer(i2c0, msgs, 1);
    
    uint8_t *ptr = buffer;

    write_buff[0] = 0x40;

    for (size_t i = 0; i < 33; i++)
    {
        for (size_t j = 1; j < 32; j++)
        {
            write_buff[j] = *ptr++;
        }

        msgs[0].addr = 0x3C;
        msgs[0].flags = 0;
        msgs[0].buffer = write_buff;
        msgs[0].length = 32;

        bflb_i2c_transfer(i2c0, msgs, 1);
    }
    
    write_data[0] = 0x40; // 
    write_data[1] = *ptr++;

    msgs[0].addr = 0x3C;
    msgs[0].flags = 0;
    msgs[0].buffer = write_data;
    msgs[0].length = 2;

    bflb_i2c_transfer(i2c0, msgs, 1);

}

// Рисование одного пикселя
void SSD1306_drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if ((x >= 0) && (x < WIDTH) && (y >= 0) && (y < HEIGHT)) {
    switch (color) {
    case SSD1306_WHITE:
      buffer[x + (y / 8) * WIDTH] |= (1 << (y & 7));
      break;
    case SSD1306_BLACK:
      buffer[x + (y / 8) * WIDTH] &= ~(1 << (y & 7));
      break;
    case SSD1306_INVERSE:
      buffer[x + (y / 8) * WIDTH] ^= (1 << (y & 7));
      break;
    }
  }
}

// Рисование горизонтальной линии
void SSD1306_drawHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
    if ((y >= 0) && (y < HEIGHT)) { // Y coord in bounds?
    if (x < 0) {                  // Clip left
      w += x;
      x = 0;
    }
    if ((x + w) > WIDTH) { // Clip right
      w = (WIDTH - x);
    }
    if (w > 0) { // Proceed only if width is positive
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x], mask = 1 << (y & 7);
      switch (color) {
      case SSD1306_WHITE:
        while (w--) {
          *pBuf++ |= mask;
        };
        break;
      case SSD1306_BLACK:
        mask = ~mask;
        while (w--) {
          *pBuf++ &= mask;
        };
        break;
      case SSD1306_INVERSE:
        while (w--) {
          *pBuf++ ^= mask;
        };
        break;
      }
    }
  }
}

// Рисование вертикальной линии               
void SSD1306_drawVLine(int16_t x, int16_t __y, int16_t __h, uint16_t color) 
{
  if ((x >= 0) && (x < WIDTH)) { // X coord in bounds?
    if (__y < 0) {               // Clip top
      __h += __y;
      __y = 0;
    }
    if ((__y + __h) > HEIGHT) { // Clip bottom
      __h = (HEIGHT - __y);
    }
    if (__h > 0) { // Proceed only if height is now positive
      // this display doesn't need ints for coordinates,
      // use local byte registers for faster juggling
      uint8_t y = __y, h = __h;
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x];

      // do the first partial byte, if necessary - this requires some masking
      uint8_t mod = (y & 7);
      if (mod) {
        // mask off the high n bits we want to set
        mod = 8 - mod;
        // note - lookup table results in a nearly 10% performance
        // improvement in fill* functions
        uint8_t mask = ~(0xFF >> mod);
        /*static const uint8_t PROGMEM premask[8] = {0x00, 0x80, 0xC0, 0xE0,
                                                   0xF0, 0xF8, 0xFC, 0xFE};
        uint8_t mask = pgm_read_byte(&premask[mod]);*/
        // adjust the mask if we're not going to reach the end of this byte
        if (h < mod)
          mask &= (0XFF >> (mod - h));

        switch (color) {
        case SSD1306_WHITE:
          *pBuf |= mask;
          break;
        case SSD1306_BLACK:
          *pBuf &= ~mask;
          break;
        case SSD1306_INVERSE:
          *pBuf ^= mask;
          break;
        }
        pBuf += WIDTH;
      }

      if (h >= mod) { // More to go?
        h -= mod;
        // Write solid bytes while we can - effectively 8 rows at a time
        if (h >= 8) {
          if (color == SSD1306_INVERSE) {
            // separate copy of the code so we don't impact performance of
            // black/white write version with an extra comparison per loop
            do {
              *pBuf ^= 0xFF; // Invert byte
              pBuf += WIDTH; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          } else {
            // store a local value to work with
            uint8_t val = (color != SSD1306_BLACK) ? 255 : 0;
            do {
              *pBuf = val;   // Set byte
              pBuf += WIDTH; // Advance pointer 8 rows
              h -= 8;        // Subtract 8 rows from height
            } while (h >= 8);
          }
        }

        if (h) { // Do the final partial byte, if necessary
          mod = h & 7;
          // this time we want to mask the low bits of the byte,
          // vs the high bits we did above
          uint8_t mask = (1 << mod) - 1;
          // note - lookup table results in a nearly 10% performance
          // improvement in fill* functions
          /*static const uint8_t PROGMEM postmask[8] = {0x00, 0x01, 0x03, 0x07,
                                                      0x0F, 0x1F, 0x3F, 0x7F};
          uint8_t mask = pgm_read_byte(&postmask[mod]);*/
          switch (color) {
          case SSD1306_WHITE:
            *pBuf |= mask;
            break;
          case SSD1306_BLACK:
            *pBuf &= ~mask;
            break;
          case SSD1306_INVERSE:
            *pBuf ^= mask;
            break;
          }
        }
      }
    } // endif positive height
  }   // endif x in bounds
}

/**************************************************************************/
/*!
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) 
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
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

  for (; x0 <= x1; x0++) {
    if (steep) {
      SSD1306_drawPixel(y0, x0, color);
    } else {
      SSD1306_drawPixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// Инверсия дисплея
void SSD1306_invertDisplay(bool i)
{
  int inv = (i ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
  
  write_data[0] = 0x00;
  write_data[1] = inv;

  msgs[0].addr = 0x3C;
  msgs[0].flags = 0;
  msgs[0].buffer = write_data;
  msgs[0].length = 2;

  bflb_i2c_transfer(i2c0, msgs, 1);
}

/**************************************************************************/
/*!
   @brief   Draw a rectangle with fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, int8_t color)
{
  if ((x >= 0) && (x + w < WIDTH) && (y >= 0) && (y + h < HEIGHT))
  {
    for (int16_t i = y; i < y + h; i++) {
    SSD1306_drawHLine(x, i, w, color);
    }
  }
}

/**************************************************************************/
/*!
   @brief    Draw a circle outline
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  SSD1306_drawPixel(x0, y0 + r, color);
  SSD1306_drawPixel(x0, y0 - r, color);
  SSD1306_drawPixel(x0 + r, y0, color);
  SSD1306_drawPixel(x0 - r, y0, color);

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    SSD1306_drawPixel(x0 + x, y0 + y, color);
    SSD1306_drawPixel(x0 - x, y0 + y, color);
    SSD1306_drawPixel(x0 + x, y0 - y, color);
    SSD1306_drawPixel(x0 - x, y0 - y, color);
    SSD1306_drawPixel(x0 + y, y0 + x, color);
    SSD1306_drawPixel(x0 - y, y0 + x, color);
    SSD1306_drawPixel(x0 + y, y0 - x, color);
    SSD1306_drawPixel(x0 - y, y0 - x, color);
  }
}

/**************************************************************************/
/*!
   @brief   Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  if ((x >= 0) && (x + w < WIDTH) && (y >= 0) && (y + h < HEIGHT))
  {
    SSD1306_drawHLine(x, y, w, color);
    SSD1306_drawHLine(x, y + h - 1, w, color);
    SSD1306_drawVLine(x, y, h, color);
    SSD1306_drawVLine(x + w - 1, y, h, color);
  }
}

/**************************************************************************/
/*!
    @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of
   the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    if (cornername & 0x4) {
      SSD1306_drawPixel(x0 + x, y0 + y, color);
      SSD1306_drawPixel(x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      SSD1306_drawPixel(x0 + x, y0 - y, color);
      SSD1306_drawPixel(x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      SSD1306_drawPixel(x0 - y, y0 + x, color);
      SSD1306_drawPixel(x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      SSD1306_drawPixel(x0 - y, y0 - x, color);
      SSD1306_drawPixel(x0 - x, y0 - y, color);
    }
  }
}

/**************************************************************************/
/*!
    @brief  Quarter-circle drawer with fill, used for circles and roundrects
    @param  x0       Center-point x coordinate
    @param  y0       Center-point y coordinate
    @param  r        Radius of circle
    @param  corners  Mask bits indicating which quarters we're doing
    @param  delta    Offset from center-point, used for round-rects
    @param  color    16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color)
{
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
      if (corners & 1)
        SSD1306_drawVLine(x0 + x, y0 - y, 2 * y + delta, color);
      if (corners & 2)
        SSD1306_drawVLine(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
      if (corners & 1)
        SSD1306_drawVLine(x0 + py, y0 - px, 2 * px + delta, color);
      if (corners & 2)
        SSD1306_drawVLine(x0 - py, y0 - px, 2 * px + delta, color);
      py = y;
    }
    px = x;
  }
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
{
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  // smarter version
  SSD1306_drawHLine(x + r, y, w - 2 * r, color);         // Top
  SSD1306_drawHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
  SSD1306_drawVLine(x, y + r, h - 2 * r, color);         // Left
  SSD1306_drawVLine(x + w - 1, y + r, h - 2 * r, color); // Right
  // draw four corners
  drawCircleHelper(x + r, y + r, r, 1, color);
  drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
  drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
  drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

/**************************************************************************/
/*!
   @brief    Draw a circle with filled color
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void SSD1306_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  SSD1306_drawVLine(x0, y0 - r, 2 * r + 1, color);
  fillCircleHelper(x0, y0, r, 3, 0, color);
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw/fill with
*/
/**************************************************************************/
void SD1306_fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
  if (r > max_radius)
    r = max_radius;
  
  SSD1306_fillRect(x + r, y, w - 2 * r, h, color);
  // draw four corners
  fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
  fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

/**************************************************************************/
/*!
   @brief    Draw a line
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  if (x0 == x1) {
    if (y0 > y1)
      _swap_int16_t(y0, y1);
     SSD1306_drawVLine(x0, y0, y1 - y0 + 1, color);
  } else if (y0 == y1) {
    if (x0 > x1)
      _swap_int16_t(x0, x1);
     SSD1306_drawHLine(x0, y0, x1 - x0 + 1, color);
  } else {
    SSD1306_writeLine(x0, y0, x1, y1, color);
  }
}

/**************************************************************************/
/*!
   @brief   Draw a triangle with no fill color
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  SSD1306_drawLine(x0, y0, x1, y1, color);
  SSD1306_drawLine(x1, y1, x2, y2, color);
  SSD1306_drawLine(x2, y2, x0, y0, color);
}

/**************************************************************************/
/*!
   @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to fill/draw with
*/
/**************************************************************************/
void SSD1306_fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                                int16_t x2, int16_t y2, uint16_t color)
{
  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_int16_t(y2, y1);
    _swap_int16_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_int16_t(y0, y1);
    _swap_int16_t(x0, x1);
  }

  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if (x1 < a)
      a = x1;
    else if (x1 > b)
      b = x1;
    if (x2 < a)
      a = x2;
    else if (x2 > b)
      b = x2;
    SSD1306_drawHLine(a, y0, b - a + 1, color);
    return;
  }

  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
          dx12 = x2 - x1, dy12 = y2 - y1;
  int32_t sa = 0, sb = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if (y1 == y2)
    last = y1; // Include y1 scanline
  else
    last = y1 - 1; // Skip it

  for (y = y0; y <= last; y++) {
    a = x0 + sa / dy01;
    b = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    SSD1306_drawHLine(a, y, b - a + 1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = (int32_t)dx12 * (y - y1);
  sb = (int32_t)dx02 * (y - y0);
  for (; y <= y2; y++) {
    a = x1 + sa / dy12;
    b = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if (a > b)
      _swap_int16_t(a, b);
    SSD1306_drawHLine(a, y, b - a + 1, color);
  }
}

/**************************************************************************/
/*!
   @brief      Draw a PROGMEM-resident 1-bit image at the specified (x,y)
   position, using the specified foreground color (unset bits are transparent).
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    bitmap  byte array with monochrome bitmap
    @param    w   Width of bitmap in pixels
    @param    h   Height of bitmap in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void SSD1306_drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                              int16_t w, int16_t h, uint16_t color) {

  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t b = 0;

  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (i & 7)
        b <<= 1;
      else
        b = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
      if (b & 0x80)
        SSD1306_drawPixel(x + i, y, color);
    }
  }
}

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
  return gfxFont->glyph + c;
}

inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
  return gfxFont->bitmap;
}


/**************************************************************************/
/*!
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel
   that much bigger.
    @param  s_x  Desired text width magnification level in X-axis. 1 is default
    @param  s_y  Desired text width magnification level in Y-axis. 1 is default
*/
/**************************************************************************/
void setTextSize(uint8_t s_x, uint8_t s_y) {
  textsize_x = (s_x > 0) ? s_x : 1;
  textsize_y = (s_y > 0) ? s_y : 1;
}

/**************************************************************************/
/*!
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel
   that much bigger.
    @param  s  Desired text size. 1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
*/
/**************************************************************************/
void SSD1306_setTextSize(uint8_t s) { setTextSize(s, s); }

/**************************************************************************/
/*!
    @brief      Set rotation setting for display
    @param  x   0 thru 3 corresponding to 4 cardinal rotations
*/
/**************************************************************************/
void SSD1306_setRotation(uint8_t x) {
  rotation = (x & 3);
  switch (rotation) {
  case 0:
  case 2:
    _width = WIDTH;
    _height = HEIGHT;
    break;
  case 1:
  case 3:
    _width = HEIGHT;
    _height = WIDTH;
    break;
  }
}

/**********************************************************************/
  /*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
  */
  /**********************************************************************/
  void SSD1306_setCursor(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
  }

  /**********************************************************************/
  /*!
    @brief   Set text font color with transparant background
    @param   c   16-bit 5-6-5 Color to draw text with
    @note    For 'transparent' background, background and foreground
             are set to same color rather than using a separate flag.
  */
  /**********************************************************************/
  void SSD1306_setTextColor(uint16_t c) { textcolor = textbgcolor = c; }


// Draw a character
/**************************************************************************/
/*!
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color,
   no background)
    @param    size_x  Font magnification level in X-axis, 1 is 'original' size
    @param    size_y  Font magnification level in Y-axis, 1 is 'original' size
*/
/**************************************************************************/
void drawChar(int16_t x, int16_t y, unsigned char c,
                            uint16_t color, uint16_t bg, uint8_t size_x,
                            uint8_t size_y) {

  if (!gfxFont) { // 'Classic' built-in font

    if ((x >= _width) ||              // Clip right
        (y >= _height) ||             // Clip bottom
        ((x + 6 * size_x - 1) < 0) || // Clip left
        ((y + 8 * size_y - 1) < 0))   // Clip top
      return;

    if (!_cp437 && (c >= 176))
      c++; // Handle 'classic' charset behavior

    for (int8_t i = 0; i < 5; i++) { // Char bitmap = 5 columns
      uint8_t line = pgm_read_byte(&font[c * 5 + i]);
      for (int8_t j = 0; j < 8; j++, line >>= 1) {
        if (line & 1) {
          if (size_x == 1 && size_y == 1)
            SSD1306_drawPixel(x + i, y + j, color);
          else
            SSD1306_fillRect(x + i * size_x, y + j * size_y, size_x, size_y,
                          color);
        } else if (bg != color) {
          if (size_x == 1 && size_y == 1)
            SSD1306_drawPixel(x + i, y + j, bg);
          else
            SSD1306_fillRect(x + i * size_x, y + j * size_y, size_x, size_y, bg);
        }
      }
    }
    if (bg != color) { // If opaque, draw vertical line for last column
      if (size_x == 1 && size_y == 1)
        SSD1306_drawVLine(x + 5, y, 8, bg);
      else
        SSD1306_fillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
    }

  } else { // Custom font

    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1) {
            SSD1306_drawPixel(x + xo + xx, y + yo + yy, color);
          } else {
            SSD1306_fillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y,
                          size_x, size_y, color);
          }
        }
        bits <<= 1;
      }
    }

  } // End classic vs custom font
}

// Draw a character
/**************************************************************************/
/*!
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color,
   no background)
    @param    size  Font magnification level, 1 is 'original' size
*/
/**************************************************************************/
void SSD1306_drawChar(int16_t x, int16_t y, unsigned char c,
                            uint16_t color, uint16_t bg, uint8_t size) {
  drawChar(x, y, c, color, bg, size, size);
}

/**************************************************************************/
/*!
    @brief  Print one byte/character of data, used to support print()
    @param  c  The 8-bit ascii character to write
*/
/**************************************************************************/
size_t write(uint8_t c) {
  if (!gfxFont) { // 'Classic' built-in font

    if (c == '\n') {              // Newline?
      cursor_x = 0;               // Reset x to zero,
      cursor_y += textsize_y * 8; // advance y one line
    } else if (c != '\r') {       // Ignore carriage returns
      if (wrap && ((cursor_x + textsize_x * 6) > _width)) { // Off right?
        cursor_x = 0;                                       // Reset x to zero,
        cursor_y += textsize_y * 8; // advance y one line
      }
      drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
               textsize_y);
      cursor_x += textsize_x * 6; // Advance x one char
    }

  } else { // Custom font

    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t w = pgm_read_byte(&glyph->width),
                h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          if (wrap && ((cursor_x + textsize_x * (xo + w)) > _width)) {
            cursor_x = 0;
            cursor_y += (int16_t)textsize_y *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
                   textsize_y);
        }
        cursor_x +=
            (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
  }
  return 1;
}

void SSD1306_print(char * str)
{
  for (size_t i = 0; str[i]; i++)
  {
    write(str[i]);
  }
}