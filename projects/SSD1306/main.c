#include "bflb_mtimer.h"
#include "bflb_i2c.h"
//#include "board.h"
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
    //SSD1306_drawPixel(10, 0, SSD1306_WHITE);

    // Рисование горизонтальной линии
    //SSD1306_drawHLine(20, 20, 50, SSD1306_WHITE);

    // Рисование вертикальной линии
    //SSD1306_drawVLine(30, 30, 10, SSD1306_WHITE);

    // Прямоугольник
    //SSD1306_drawRect(10, 10, 50, 20, SSD1306_WHITE);

    // Залитый прямоугольник
    //SSD1306_fillRect(10, 10, 50, 20, SSD1306_WHITE);
    
    // Окружность
    //SSD1306_drawCircle(40, 40, 20, SSD1306_WHITE);

    // Скругленный прямоугольник
    //SSD1306_drawRoundRect(20, 20, 50, 30, 5, SSD1306_WHITE);

    //SSD1306_fillCircle(40, 40, 20, SSD1306_WHITE);

    //SD1306_fillRoundRect(20, 20, 50, 30, 10, SSD1306_WHITE);

    //SSD1306_drawLine(10, 10, 120, 60, SSD1306_WHITE);

    //SSD1306_drawTriangle(10, 10, 80, 30, 40, 60, SSD1306_WHITE);

    //SSD1306_fillTriangle(10, 10, 80, 30, 40, 60, SSD1306_WHITE);

    /*SSD1306_drawBitmap((WIDTH - splash1_width) / 2, (HEIGHT - splash1_height) / 2,
               splash1_data, splash1_width, splash1_height, 1);*/


    //_cp437 = false;

    SSD1306_setRotation(0);
    SSD1306_setTextSize(1);
    SSD1306_setCursor(10, 10);
    SSD1306_setTextColor(SSD1306_WHITE);

    SSD1306_print("1234567890");

    
    
    // Вывод буфера на дисплей
    SSD1306_display();



}
