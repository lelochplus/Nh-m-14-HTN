#include <stdint.h>

/* ================= RCC ================= */

#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

/* Bật clock cho GPIOA */
#define IOPAEN          (1U << 2)


/* ================= GPIOA ================= */

#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

/* Chân PA2 */
#define PA2             (1U << 2)


/* ================= DELAY ================= */

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


/* ================= MAIN ================= */

int main(void)
{
    /* Bật clock cho GPIOA */
    RCC_APB2ENR |= IOPAEN;

    /*
     * Cấu hình PA2:
     * MODE = 01 -> Output 10 MHz
     * CNF  = 00 -> Push-Pull
     */
    GPIOA_CRL &= ~(0xFU << 8);
    GPIOA_CRL |=  (0x1U << 8);

    /* Ban đầu PA2 = 0 */
    GPIOA_ODR &= ~PA2;

    while (1)
    {
        /* Đảo trạng thái LED */
        GPIOA_ODR ^= PA2;

        /* Delay khoảng 1 giây */
        delay_ms(500);
    }
}
