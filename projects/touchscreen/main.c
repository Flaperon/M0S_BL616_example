#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "board.h"
#include "ft6x36_i2c.h"
#include "touch.h"

struct bflb_device_s *gpio;

touch_coord_t maxval[1];
touch_coord_t mypoint[2]; // Координаты точек
uint8_t pn;

void gpio_isr(int irq, void *arg)
{
    bool intstatus = bflb_gpio_get_intstatus(gpio, GPIO_PIN_0);
    if (intstatus) {
        bflb_gpio_int_clear(gpio, GPIO_PIN_0);
        ft6x36_i2c_read(&pn, mypoint, 2);
        printf("%d\r\n", mypoint[1].coord_y);
    }
}

int main(void)
{
    board_init();

    maxval[0].coord_x = 128;
    maxval[0].coord_y = 150;

    ft6x36_i2c_init(maxval); // Инициализация сенсора

    gpio = bflb_device_get_by_name("gpio");
    printf("gpio interrupt\r\n");

    bflb_gpio_int_init(gpio, GPIO_PIN_0, GPIO_INT_TRIG_MODE_SYNC_LOW_LEVEL);
    bflb_gpio_int_mask(gpio, GPIO_PIN_0, false);

    bflb_irq_attach(gpio->irq_num, gpio_isr, gpio);
    bflb_irq_enable(gpio->irq_num);

    while (1) {
        //printf("helloworld\r\n");
        //bflb_mtimer_delay_ms(2000);
    }
}
