#include <stdint.h>


/* =========================
   RCC - bộ điều khiển clock
   ========================= */

#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

/* Clock cho AFIO */
#define AFIOEN          (1U << 0)

/* Clock cho GPIOA */
#define IOPAEN          (1U << 2)

/* Clock cho GPIOB */
#define IOPBEN          (1U << 3)


/* =========================
   AFIO
   ========================= */

#define AFIO_BASE       0x40010000UL
#define AFIO_MAPR       (*(volatile uint32_t *)(AFIO_BASE + 0x04))


/* =========================
   GPIOA
   ========================= */

#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))


/* =========================
   GPIOB
   ========================= */

#define GPIOB_BASE      0x40010C00UL

#define GPIOB_CRL       (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_ODR       (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))


/* =========================
   Khởi tạo GPIO
   ========================= */

void gpio_init(void)
{
    /* =========================
       Bật clock
       ========================= */

    RCC_APB2ENR |= IOPAEN | IOPBEN | AFIOEN;


    /* =========================
       Tắt JTAG
       Giữ SWD cho ST-Link
       ========================= */

    AFIO_MAPR &= ~(7U << 24);
    AFIO_MAPR |=  (2U << 24);


    /* =========================
       PA0 - PA7
       Input Pull-up
       ========================= */

    GPIOA_CRL = 0x88888888U;

    /* Bật điện trở kéo lên */
    GPIOA_ODR |= 0x00FFU;


    /* =========================
       PA8 - PA12 và PA15
       Output Push-Pull
       ========================= */

    /* Xóa cấu hình cũ */
    GPIOA_CRH &= ~(
        (0xFU << 0)  |   /* PA8  */
        (0xFU << 4)  |   /* PA9  */
        (0xFU << 8)  |   /* PA10 */
        (0xFU << 12) |   /* PA11 */
        (0xFU << 16) |   /* PA12 */
        (0xFU << 28)     /* PA15 */
    );


    /* Cấu hình Output Push-Pull 10MHz */
    GPIOA_CRH |= (
        (0x1U << 0)  |   /* PA8  */
        (0x1U << 4)  |   /* PA9  */
        (0x1U << 8)  |   /* PA10 */
        (0x1U << 12) |   /* PA11 */
        (0x1U << 16) |   /* PA12 */
        (0x1U << 28)     /* PA15 */
    );


    /* =========================
       PB3 và PB4
       Output Push-Pull 10MHz
       ========================= */

    GPIOB_CRL &= ~(
        (0xFU << 12) |   /* PB3 */
        (0xFU << 16)     /* PB4 */
    );

    GPIOB_CRL |= (
        (0x1U << 12) |   /* PB3 */
        (0x1U << 16)     /* PB4 */
    );


    /* =========================
       Tắt tất cả LED ban đầu
       ========================= */

    GPIOA_ODR &= ~(
        (1U << 8)  |
        (1U << 9)  |
        (1U << 10) |
        (1U << 11) |
        (1U << 12) |
        (1U << 15)
    );

    GPIOB_ODR &= ~(
        (1U << 3) |
        (1U << 4)
    );
}


/* =========================
   Chương trình chính
   ========================= */

int main(void)
{
    uint32_t input;


    /* Khởi tạo GPIO */
    gpio_init();


    /* =========================
       Vòng lặp chính
       ========================= */

    while (1)
    {
        /* Đọc PA0 - PA7 */
        input = GPIOA_IDR & 0x00FFU;


        /* =========================
           Đảo trạng thái Input
           ========================= */

        input = (~input) & 0x00FFU;


        /* =========================
           PA0 → PA8
           ========================= */

        if (input & (1U << 0))
            GPIOA_ODR |= (1U << 8);
        else
            GPIOA_ODR &= ~(1U << 8);


        /* =========================
           PA1 → PA9
           ========================= */

        if (input & (1U << 1))
            GPIOA_ODR |= (1U << 9);
        else
            GPIOA_ODR &= ~(1U << 9);


        /* =========================
           PA2 → PA10
           ========================= */

        if (input & (1U << 2))
            GPIOA_ODR |= (1U << 10);
        else
            GPIOA_ODR &= ~(1U << 10);


        /* =========================
           PA3 → PA11
           ========================= */

        if (input & (1U << 3))
            GPIOA_ODR |= (1U << 11);
        else
            GPIOA_ODR &= ~(1U << 11);


        /* =========================
           PA4 → PA12
           ========================= */

        if (input & (1U << 4))
            GPIOA_ODR |= (1U << 12);
        else
            GPIOA_ODR &= ~(1U << 12);


        /* =========================
           PA5 → PA15
           ========================= */

        if (input & (1U << 5))
            GPIOA_ODR |= (1U << 15);
        else
            GPIOA_ODR &= ~(1U << 15);


        /* =========================
           PA6 → PB3
           ========================= */

        if (input & (1U << 6))
            GPIOB_ODR |= (1U << 3);
        else
            GPIOB_ODR &= ~(1U << 3);


        /* =========================
           PA7 → PB4
           ========================= */

        if (input & (1U << 7))
            GPIOB_ODR |= (1U << 4);
        else
            GPIOB_ODR &= ~(1U << 4);
    }
}
