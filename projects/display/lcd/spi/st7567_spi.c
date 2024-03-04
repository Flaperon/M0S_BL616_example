/**
 * @file st7567_spi.c
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

#if defined(LCD_SPI_ST7567)

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
#if (ST7567_SPI_PIXEL_FORMAT == 1)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_RGB565,
#elif (ST7567_SPI_PIXEL_FORMAT == 2)
    .pixel_format = LCD_SPI_LCD_PIXEL_FORMAT_NRGB8888,
#endif
};

#else

#error "Configuration error"

#endif

#ifdef LCD_SPI_ST7567
const st7567_spi_init_cmd_t st7567_spi_init_cmds[] = {
    { 0xE2, NULL, 0 }, /* Software reset */
    { 0xAE, NULL, 0 }, /* Display OFF */
    { 0x40, NULL, 0 }, /* Set start line 00 */
    { 0xA1, NULL, 0 }, /* SEG direction, MX=1, reverse direction */
    { 0xC0, NULL, 0 }, /* COM direction, MY=0, normal direction */
    { 0xA6, NULL, 0 }, /* Inverse display, INV=0, normal display */
    { 0xA2, NULL, 0 }, /* Bias select, 0 -> 1/9 (A3 -> 1/7) */
    { 0x2C, NULL, 0 }, /* Power control, VB=1, VR=0, VF=0 */
    { 0xFF, NULL, 50 },
    { 0x2E, NULL, 0 }, /* Power control, VB=1, VR=1, VF=0 */
    { 0xFF, NULL, 50 },
    { 0x2F, NULL, 0 }, /* Power control, VB=1, VR=1, VF=1 */
    { 0xFF, NULL, 50 },
    { 0x23, NULL, 0 }, /* Regulation resistor ratio, RR2=0, RR1=1, RR0=1 */
    { 0x81, "\x32", 1 }, /* Contrast, EV5=1, EV4=1, EV3=0, EV2=0, EV1=1, EV=0 */
    { 0xAE, NULL, 0 }, /* Display OFF */
    { 0xA5, NULL, 0 } /* Power save mode */

    //{ 0xA4, NULL, 0 } /* Power save mode OFF */
    //{ 0xAF, NULL, 0 } /* Display ON */
};
#endif



/**
 * @brief st7567_spi_async_callback_enable
 *
 * @return
 */
void st7567_spi_async_callback_enable(bool enable)
{
    lcd_spi_sync_callback_enable(enable);
}

/**
 * @brief st7567_spi_async_callback_register
 *
 * @return
 */
void st7567_spi_async_callback_register(void (*callback)(void))
{
    lcd_spi_async_callback_register(callback);
}

/**
 * @brief st7725_spi_draw_is_busy, After the call st7567_spi_draw_picture_dma must check this,
 *         if st7567_spi_draw_is_busy() == 1, Don't allow other draw !!
 *         can run in the DMA interrupt callback function.
 *
 * @return int 0:draw end; 1:Being draw
 */
int st7567_spi_draw_is_busy(void)
{
    return lcd_spi_isbusy();
}

/**
 * @brief st7567_spi_init
 *
 * @return int
 */
int st7567_spi_init()
{
    lcd_spi_init(&spi_para);

    for (uint16_t i = 0; i < (sizeof(st7567_spi_init_cmds) / sizeof(st7567_spi_init_cmds[0])); i++) {
        if (st7567_spi_init_cmds[i].cmd == 0xFF && st7567_spi_init_cmds[i].data == NULL && st7567_spi_init_cmds[i].databytes) {
            bflb_mtimer_delay_ms(st7567_spi_init_cmds[i].databytes);
        } else {
            lcd_spi_transmit_cmd_para(st7567_spi_init_cmds[i].cmd, (void *)(st7567_spi_init_cmds[i].data), st7567_spi_init_cmds[i].databytes);
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
int st7567_spi_set_dir(uint8_t dir, uint8_t mir_flag)
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
 * @brief st7567_spi_set_draw_window
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void st7567_spi_set_draw_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
#if st7567_SPI_OFFSET_X
    x1 += st7567_SPI_OFFSET_X;
    x2 += st7567_SPI_OFFSET_X;
#endif
#if st7567_SPI_OFFSET_Y
    y1 += st7567_SPI_OFFSET_Y;
    y2 += st7567_SPI_OFFSET_Y;
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
 * @brief st7567_spi_draw_point
 *
 * @param x
 * @param y
 * @param color
 */
void st7567_spi_draw_point(uint16_t x, uint16_t y, st7567_spi_color_t color)
{
    /* set window */
    st7567_spi_set_draw_window(x, y, x, y);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)&color, 1);
}

/**
 * @brief st7567_draw_area
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param color
 */
void st7567_spi_draw_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7567_spi_color_t color)
{
    uint32_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7567_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_fill_sync(0x2C, (uint32_t)color, pixel_cnt);
}

/**
 * @brief st7567_draw_picture_dma, Non-blocking! Using DMA acceleration, Not waiting for the draw end
 *  After the call, No other operations are allowed until (st7567_draw_is_busy()==0)
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7567_spi_draw_picture_nonblocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7567_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7567_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_async(0x2C, (void *)picture, pixel_cnt);
}

/**
 * @brief st7567_draw_picture,Blocking,Using DMA acceleration,Waiting for the draw end
 *
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param picture
 */
void st7567_spi_draw_picture_blocking(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, st7567_spi_color_t *picture)
{
    size_t pixel_cnt = (x2 - x1 + 1) * (y2 - y1 + 1);

    /* set window */
    st7567_spi_set_draw_window(x1, y1, x2, y2);

    lcd_spi_transmit_cmd_pixel_sync(0x2C, (void *)picture, pixel_cnt);
}

#endif
