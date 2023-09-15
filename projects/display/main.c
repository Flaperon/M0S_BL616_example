#include "board.h"
#include "lcd/lcd.h"
#include "lcd/lcd_conf.h"

#define STRING1 "Hello, World!\n"

int main(void)
{
    board_init();
    lcd_init();
    
    lcd_clear(BLACK);
    lcd_set_dir(0,0);
    lcd_draw_str_ascii16(10,10,WHITE,BLACK,STRING1,strlen(STRING1));
    
    while (1) {
        
    }
}
