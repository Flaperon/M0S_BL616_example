#include "SSD1306.h"
#include "board.h"
#include "bflb_gpio.h"
#include "bflb_i2c.h"

static struct bflb_device_s *i2c0;

struct bflb_i2c_msg_s msgs[1];

uint8_t write_data[7]; // Массив для работы с командами
uint8_t write_buff[32]; // Массив для работы с буфером

uint8_t *buffer;

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

void SSD1306_drawPixel(int16_t x, int16_t y, uint16_t color)
{
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

