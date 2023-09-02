#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

static struct bflb_device_s *i2c0;

int i2c_addr;
uint8_t write_data[3];

int main(void)
{
    struct bflb_i2c_msg_s msgs[2];

    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);

    write_data[0] = 0xff;
    write_data[1] = 0xcc;
    write_data[2] = 0xdd;


    //for (size_t i = 0; i < 120; i++)
    //{
        msgs[0].addr = 0x3C;
        msgs[0].flags = 0;
        msgs[0].buffer = write_data;
        msgs[0].length = 2;

        /*msgs[1].addr = 0x3C;
        msgs[1].flags = 0;
        msgs[1].buffer = write_data;
        msgs[1].length = 3;*/

        int err = bflb_i2c_transfer(i2c0, msgs, 1);

        printf("Return = %d\r\n", err);

        //bflb_mtimer_delay_ms(100);
        
    //}

    while(1){
    }
}
