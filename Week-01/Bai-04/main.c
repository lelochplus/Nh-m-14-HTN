#include <stdint.h>

/* ================= RCC ================= */
#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define IOPAEN          (1U << 2)

/* ================= GPIOA ================= */
#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define PA0             (1U << 0)
#define PA7             (1U << 7)

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
    /* Bật clock GPIOA */
    RCC_APB2ENR |= IOPAEN;

    /*
     * PA0:
     * Input Pull-up/Pull-down
     * CNF = 10
     * MODE = 00
     * => 0x8
     */
    GPIOA_CRL &= ~(0xFU << 0);
    GPIOA_CRL |=  (0x8U << 0);

    /* Kéo PA0 lên mức HIGH */
    GPIOA_ODR |= PA0;

    /*
     * PA7:
     * Output Push-Pull
     * MODE = 01 (10 MHz)
     * CNF  = 00
     * => 0x1
     */
    GPIOA_CRL &= ~(0xFU << 28);
    GPIOA_CRL |=  (0x1U << 28);

    /* LED ban đầu tắt */
    GPIOA_ODR &= ~PA7;
}

/* ================= MAIN ================= */
int main(void)
{
    uint8_t led_state = 0;

    gpio_init();

    while (1)
    {
        /* Phát hiện nhấn nút */
        if ((GPIOA_IDR & PA0) == 0)
        {
            /* Chống dội phím */
            delay_ms(20);

            if ((GPIOA_IDR & PA0) == 0)
            {
                /*
                 * Chờ nhả nút.
                 * LED chưa đổi trong lúc đang giữ nút.
                 */
                while ((GPIOA_IDR & PA0) == 0)
                {
                }

                /* Chống dội khi nhả */
                delay_ms(20);

                /* Đảo trạng thái LED */
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
