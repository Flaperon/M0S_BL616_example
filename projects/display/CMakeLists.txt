cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# lcd driver
target_sources(app PRIVATE lcd/lcd.c)
target_sources(app PRIVATE lcd/font.c)

target_sources(app PRIVATE lcd/mipi_dbi/bl_mipi_dbi_typeb_pec.c)
target_sources(app PRIVATE lcd/mipi_dbi/bl_mipi_dbi.c)
target_sources(app PRIVATE lcd/mipi_dbi/ili9488_dbi.c)
target_sources(app PRIVATE lcd/mipi_dbi/ili9341_dbi.c)

target_sources(app PRIVATE lcd/spi/bl_spi_hard_4.c)
target_sources(app PRIVATE lcd/spi/ili9341_spi.c)
target_sources(app PRIVATE lcd/spi/ili9488_spi.c)
target_sources(app PRIVATE lcd/spi/st7735_spi.c)
target_sources(app PRIVATE lcd/spi/st7796_spi.c)
target_sources(app PRIVATE lcd/spi/st7789v_spi.c)

target_sources(app PRIVATE lcd/mipi_dpi/bl_mipi_dpi_pec.c)
target_sources(app PRIVATE lcd/mipi_dpi/standard_dpi.c)
target_sources(app PRIVATE lcd/mipi_dpi/ili9488_dpi.c)
target_sources(app PRIVATE lcd/mipi_dpi/gc9503v_dpi.c)
sdk_add_include_directories(lcd)

sdk_set_main_file(main.c)

project(display)
