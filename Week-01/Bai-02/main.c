#include <stdint.h>

/* ================= RCC ================= */

#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define IOPAEN          (1U << 2)


/* ================= GPIOA ================= */

#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))


/* ================= Delay ================= */

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


/* ================= GPIO Init ================= */

void gpio_init(void)
{
    /* Enable clock for GPIOA */
    RCC_APB2ENR |= IOPAEN;

    /*
     * PA0 -> PA7
     *
     * Mỗi chân:
     * MODE = 01 -> Output 10 MHz
     * CNF  = 00 -> General purpose push-pull
     *
     * Giá trị cấu hình = 0x1
     */

    GPIOA_CRL = 0x11111111;

    /* Tắt tất cả LED ban đầu */
    GPIOA_ODR &= ~0xFFU;
}


/* ================= Main ================= */

int main(void)
{
    gpio_init();

    while (1)
    {
        /* ================= PA0 -> PA7 ================= */

        for (int i = 0; i < 8; i++)
        {
            GPIOA_ODR = (1U << i);

            delay_ms(100);
        }


        /* ================= PA7 -> PA0 ================= */

        for (int i = 6; i > 0; i--)
        {
            GPIOA_ODR = (1U << i);

            delay_ms(100);
        }

        /*
         * Sau khi PA1 sáng,
         * vòng lặp quay lại PA0.
         */
    }
}
