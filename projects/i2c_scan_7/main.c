#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "board.h"

static struct bflb_device_s *i2c0;

int i2c_addr;

int main(void)
{
    struct bflb_i2c_msg_s msgs[1];

    board_init();
    board_i2c0_gpio_init();

    i2c0 = bflb_device_get_by_name("i2c0");

    bflb_i2c_init(i2c0, 400000);

    for (size_t i = 0; i < 127; i++)
    {
        msgs[0].addr = i;
        //msgs[0].flags = I2C_M_NOSTOP | I2C_M_TEN;
        //msgs[0].flags = I2C_M_READ; // уточнить - м.б. просто 0?
        //msgs[0].buffer = 0; // уточнить! массив?
        msgs[0].length = 1;

        int err = bflb_i2c_transfer(i2c0, msgs, 1);

        if (err >= 0)
        {
            printf("Found device: %d\r\n", i);
        }
        else
        {
            printf("%d\r\n", i);
        }
        
        
    }

    while(1){
    }
}
