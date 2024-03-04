/**
 * @file st7735_spi.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "../lcd.h"

#if defined(LCD_SPI_ST7735)

#if (LCD_SPI_INTERFACE_TYPE == 1)
#include "bl_spi_hard_4.h"

#define lcd_spi_init                          lcd_spi_hard_4_init
#define lcd_spi_isbusy                        lcd_spi_hard_4_is_busy

#define lcd_spi_transmit_cmd_para             lcd_spi_hard_4_transmit_cmd_para
#define lcd_spi_transmit_cmd_pixel_sync       lcd_spi_hard_4_transmit_cmd_pixel_sync
#define lcd_spi_transmit_cmd_pixel_fill_sync  lcd_spi_hard_4_transmit_cmd_pixel_fill_sync

#define lcd_spi_sync_callback_enable          lcd_spi_hard_4_async_callback_enable
#define lcd_spi_async_callback_register       lcd_spi_hard_4_async_callback_register
#define lcd_spi_transmit_cmd_pixel_async      lcd_spi_hard_4_transmit_cmd_pixel_async
#define lcd_spi_transmit_cmd_pixel_fill_async lcd_spi_hard_4_transmit_cmd_pixel_fill_async

static lcd_spi_hard_4_init_t spi_para = {
    .clock_freq = 40 * 1000 * 1000,
#if (ST7735_SPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_RGB565,
#elif (ST7735_SPI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#else

#error "Configuration error"

#endif

#ifdef ST7735_BLACKTAB
const st7735_spi_init_cmd_t st7735_spi_init_cmds[] = {
    { 0x01, NULL, 0 }, /* Software reset*/
    { 0xFF, NULL, 50 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 200 },

    { 0x3A, "\x05", 1 }, /* Set color mode: 16 bit color*/
    { 0xFF, NULL, 10 }, /* Delay 10 ms */

    { 0xB1, "\x00\x06\x03", 3 }, /* Framerate ctrl - normal mode */
    { 0xFF, NULL, 10 }, /* Delay 10 ms */

    { 0x36, "\x08", 1 }, /* Mem access ctl (directions) */

    { 0xB6, "\x15\x02", 2 }, /* Display settings #5, 2 args */
    { 0xB4, "\x00", 1 }, /* Display inversion ctrl */  

    { 0xC0, "\x02\x70", 2 }, /* Power control */
    { 0xFF, NULL, 10 }, /* Delay 10 ms */
    { 0xC1, "\x05", 1 }, /* Power control */
    { 0xC2, "\x01\x02", 2 }, /* Power control */
    { 0xC5, "\x3C\x38", 2 }, /* Power control */
    { 0xFF, NULL, 10 }, /* Delay 10 ms */

    { 0xFC, "\x11\x15", 2 }, /* Power control */

    { 0xE0, "\x09\x16\x09\x20\x21\x1B\x13\x19\x17\x15\x1E\x2B\x04\x05\x02\x0E", 16 },
    { 0xE1, "\x0B\x14\x08\x1E\x22\x1D\x18\x1E\x1B\x1A\x24\x2B\x06\x06\x02\x0F", 16 },
    { 0xFF, NULL, 10 }, /* Delay 10 ms */

    { 0x2A, "\x00\x02\x00\x7F", 4 }, /* Column addr set (1-128) */
    { 0x2B, "\x00\x02\x00\x9F", 4 }, /* Row addr set (1-160) */

    { 0x13, NULL, 0 }, /* Normal display on */
    { 0xFF, NULL, 10 },
    { 0x29, NULL, 0 }, /* Main screen turn on */
    { 0xFF, NULL, 255 },

};
#endif

#if defined ST7735_GREENTAB
const st7735_spi_init_cmd_t st7735_spi_init_cmds[] = {
    { 0x01, NULL, 0 }, /* Software reset*/
    { 0xFF, NULL, 150 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 200 },

    { 0xB1, "\x01\x2C\x2D", 3 }, /* Framerate ctrl - normal mode */
    { 0xB2, "\x01\x2C\x2D", 3 }, /* Framerate ctrl - idle mode */
    { 0xB3, "\x01\x2C\x2D\x01\x2C\x2D", 6 }, /* Framerate - partial mode */
    { 0xB4, "\x07", 1 }, /* Display inversion ctrl */

    { 0xC0, "\xA2\x02\x84", 3 }, /* Power control */
    { 0xC1, "\xC5", 1 }, /* Power control */
    { 0xC2, "\x0A\x00", 2 }, /* Power control */
    { 0xC3, "\x8A\x2A", 2 }, /* Power control */
    { 0xC4, "\x8A\xEE", 2 }, /* Power control */
    { 0xC5, "\x0E", 1 }, /* Power control */

    { 0x20, NULL, 0 }, /* Don't invert display */

    { 0x36, "\xA0", 1 }, /* Mem access ctl (directions) */

    { 0x3A, "\x05", 1 }, /* Set color mode: 16 bit color*/

    /* Specific for RED tab display*/
    { 0x2A, "\x00\x00\x00\x7F", 4 }, /* Column addr set (0-127) */
    { 0x2B, "\x00\x00\x00\x9F", 4 }, /* Row addr set (0-159) */

    /* RED or GREEN tab */
    { 0xE0, "\x02\x1C\x07\x12\x37\x32\x29\x2D\x29\x25\x2B\x39\x00\x01\x03\x10", 16 },
    { 0xE1, "\x03\x1D\x07\x06\x2E\x2C\x29\x2D\x2E\x2E\x37\x3F\x00\x00\x02\x10", 16 },

    { 0x13, NULL, 0 }, /* Normal display on */
    { 0xFF, NULL, 10 },
    { 0x29, NULL, 0 }, /* Main screen turn on */
    { 0xFF, NULL, 100 },
};
#endif

#if defined ST7735_80_160
const st7735_spi_init_cmd_t st7735_spi_init_cmds[] = {
    { 0x01, NULL, 0 }, /* Software reset*/
    { 0xFF, NULL, 150 },
    { 0x11, NULL, 0 }, /* Exit sleep */
    { 0xFF, NULL, 255 },

    { 0xB1, "\x01\x2C\x2D", 3 }, /* Framerate ctrl - normal mode */
    { 0xB2, "\x01\x2C\x2D", 3 }, /* Framerate ctrl - idle mode */
    { 0xB3, "\x01\x2C\x2D\x01\x2C\x2D", 6 }, /* Framerate - partial mode */
    { 0xB4, "\x07", 1 }, /* Display inversion ctrl */

    { 0xC0, "\xA2\x02\x84", 3 }, /* Power control */
    { 0xC1, "\xC5", 1 }, /* Power control */
    { 0xC2, "\x0A\x00", 2 }, /* Power control */
    { 0xC3, "\x8A\x2A", 2 }, /* Power control */
    { 0xC4, "\x8A\xEE", 2 }, /* Power control */
    { 0xC5, "\x0E", 1 }, /* Power control */

    { 0x20, NULL, 0 }, /* Don't invert display */

    { 0x36, "\xC8", 1 }, /* Mem access ctl (directions) */

    { 0x3A, "\x05", 1 }, /* Set color mode: 16 bit color*/

    /* Part 2 - Specific for 160x80 plugin*/
    { 0x21, NULL, 0 }, /* Invert display */
    { 0x2A, "\x00\x00\x00\x4F", 4 }, /* Column addr set (0-79) */
    { 0x2B, "\x00\x00\x00\x9F", 4 }, /* Row addr set (0-159) */

    /* Part 3 */
    { 0xE0, "\x02\x1C\x07\x12\x37\x32\x29\x2D\x29\x25\x2B\x39\x00\x01\x03\x10", 16 },
    { 0xE1, "\x03\x1D\x07\x06\x2E\x2C\x29\x2D\x2E\x2E\x37\x3F\x00\x00\x02\x10", 16 },

    { 0x13, NULL, 0 }, /* Normal display on */
    { 0xFF, NULL, 10 },
    { 0x29, NULL, 0 }, /* Main screen turn on */
    { 0xFF, NULL, 100 },
};
#endif

/**
 * @brief st7735_spi_async_callback_enable
 *
 * @return
 */
void st7735_spi_async_callback_enable(bool enable)
{
    lcd_spi_sync_callback_enable(enable);
}

/**
 * @brief st7735_spi_async_callback_register
 *
 * @return
 */
void st7735_spi_async_callback_register(void (*callback)(void))
{
    lcd_spi_async_callback_register(callback);
}

/**
 * @brief st7725_spi_draw_is_busy, After the call st7735_spi_draw_picture_dma must check this,
 *         if st7735_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st7735_spi_draw_is_busy(void)
{
    return lcd_spi_isbusy();
}

/**
 * @brief st7735_spi_init
 *
 * @return int
 */
int st7735_spi_init()
{
    lcd_spi_init(&spi_para);

    for (uint16_t i = 0; i < (sizeof(st7735_spi_init_cmds) / sizeof(st7735_spi_init_cmds[0])); i++) {
        if (st7735_spi_init_cmds[i].cmd == 0xFF && st7735_spi_init_cmds[i].data == NULL && st7735_spi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(st7735_spi_init_cmds[i].databytes);
        } else {
            lcd_spi_transmit_cmd_para(st7735_spi_init_cmds[i].cmd, (void *)(st7735_spi_init_cmds[i].data), st7735_spi_init_cmds[i].databytes);
        }
    }

    return 0;
}

/**
 * @brief
 *
 * @param dir
 * @param mir_flag
 */
int st7735_spi_set_dir(uint8_t dir, uint8_t mir_flag)
{
    uint8_t param;

    switch (dir) {
        case 0:
            if (!mir_flag)
                param = 0x00;
            else
                param = 0x40;
            break;
        case 1:
            if (!mir_flag)
                param = 0xA0;
            else
                param = 0x20;
            break;
        case 2:
            if (!mir_flag)
                param = 0xC0;
            else
                param = 0x80;
            break;
        case 3:
            if (!mir_flag)
                param = 0x60;
            else
                param = 0xE0;

            break;
        default:
            return -1;
            break;
    }

    lcd_spi_transmit_cmd_para(0x36, (void *)&param, 1);

    return dir;
}

/**
 * @brief st7735_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void st7735_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if ST7735_SPI_OFFSET_X
    x1 += ST7735_SPI_OFFSET_X;
    x2 += ST7735_SPI_OFFSET_X;
#endif
#if ST7735_SPI_OFFSET_Y
    y1 += ST7735_SPI_OFFSET_Y;
    y2 += ST7735_SPI_OFFSET_Y;
#endif

    int8_t param[4];

    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;

    lcd_spi_transmit_cmd_para(0x2A, (void *)param, 4);

    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;

    lcd_spi_transmit_cmd_para(0x2B, (void *)param, 4);
}

/**
 * @brief st7735_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st7735_spi_draw_point(uint16_t x, uint16_t y, st7735_spi_color_t color)
{
    /* set window */
    st7735_spi_set_draw_window(x, y, x, y);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief st7735_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st7735_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7735_spi_color_t color)
{
    uint32_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7735_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_fill_sync(0x2C, (uint32_t)color, pixel_cnt);
}

/**
 * @brief st7735_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st7735_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7735_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7735_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7735_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_cnt);
}

/**
 * @brief st7735_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7735_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7735_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7735_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_cnt);
}

#endif
