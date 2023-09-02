#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"
//#include "SSD1306.h"
#include "SSD1306.c"


int main(void)
{
    board_init();
    
    // Инициализация дисплея
    SSD1306_init();

    // Очистка буфера
    SSD1306_clear();

    // Рисование точки
    SSD1306_drawPixel(10, 0, SSD1306_WHITE);

    // Рисование горизонтальной линии
    SSD1306_drawHLine(20, 20, 50, SSD1306_WHITE);

    // Рисование вертикальной линии
    SSD1306_drawVLine(30, 30, 10, SSD1306_WHITE);

    // Вывод буфера на дисплей
    SSD1306_display();
}