#include <stdint.h>


#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define IOPAEN          (1U << 2)



#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define PA2             (1U << 2)



void delay_ms(uint32_t ms)
{
    volatile uint32_t i;

    while (ms--)
    {
        for (i = 0; i < 800; i++)
        {
            __asm volatile ("nop");
        }
    }
}



int main(void)
{
    RCC_APB2ENR |= IOPAEN;

    GPIOA_CRL &= ~(0xFU << 8);
    GPIOA_CRL |=  (0x1U << 8);

    GPIOA_ODR &= ~PA2;

    while (1)
    {
        GPIOA_ODR ^= PA2;

        delay_ms(500);
    }
}
