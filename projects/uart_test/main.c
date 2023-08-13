#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

int main(void)
{
    board_init();

    while (1) {

        bl_show_log();
        
        bflb_mtimer_delay_ms(1000);
    }
}
