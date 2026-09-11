#include <stdint.h>


#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define IOPAEN          (1U << 2)



#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))



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



    GPIOA_CRL = 0x11111111;

    GPIOA_ODR &= ~0xFFU;
}



int main(void)
{
    gpio_init();

    while (1)
    {

        for (int i = 0; i < 8; i++)
        {
            GPIOA_ODR = (1U << i);

            delay_ms(100);
        }



        for (int i = 6; i > 0; i--)
        {
            GPIOA_ODR = (1U << i);

            delay_ms(100);
        }

      
    }
}
