#include <stdint.h>

#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define IOPAEN          (1U << 2)

#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define PA0             (1U << 0)
#define PA7             (1U << 7)

void delay_ms(uint32_t ms)
{
    volatile uint32_t i;

    while (ms--)
    {
        for (i = 0; i < 8000; i++)
        {
            __asm volatile ("nop");
        }
    }
}

void gpio_init(void)
{
    RCC_APB2ENR |= IOPAEN;

   
    GPIOA_CRL &= ~(0xFU << 0);
    GPIOA_CRL |=  (0x8U << 0);

    GPIOA_ODR |= PA0;

   
    GPIOA_CRL &= ~(0xFU << 28);
    GPIOA_CRL |=  (0x1U << 28);

    GPIOA_ODR &= ~PA7;
}

int main(void)
{
    uint8_t led_state = 0;

    gpio_init();

    while (1)
    {
        if ((GPIOA_IDR & PA0) == 0)
        {
            delay_ms(20);

            if ((GPIOA_IDR & PA0) == 0)
            {
             
                while ((GPIOA_IDR & PA0) == 0)
                {
                }

                delay_ms(20);

                led_state = !led_state;

                if (led_state)
                {
                    GPIOA_ODR |= PA7;
                }
                else
                {
                    GPIOA_ODR &= ~PA7;
                }
            }
        }
    }
}
