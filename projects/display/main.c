#include "board.h"
#include "lcd/lcd.h"
#include "lcd/lcd_conf_user.h"

#define STRING1 "Hello, World!\n"

int main(void)
{
    board_init();
    lcd_init();
    
    lcd_clear(BLACK);
    lcd_set_dir(3,0);
    lcd_draw_str_ascii16(10,10,WHITE,BLACK,STRING1,strlen(STRING1));
    //lcd_draw_rectangle(0,0,79,159,WHITE);
    
    while (1) {
        
    }
}
 