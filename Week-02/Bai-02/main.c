#include <stdint.h>

/* Địa chỉ RCC */
#define RCC_BASE      (0x40021000UL)
#define RCC_APB2ENR   (*(volatile uint32_t *)(RCC_BASE + 0x18UL))

/* Địa chỉ GPIOA */
#define GPIOA_BASE    (0x40010800UL)
#define GPIOA_CRL     (*(volatile uint32_t *)(GPIOA_BASE + 0x00UL))
#define GPIOA_ODR     (*(volatile uint32_t *)(GPIOA_BASE + 0x0CUL))

/* Cấu hình SysTick */
#define SCS_BASE      (0xE000E000UL)
#define SysTick_BASE  (SCS_BASE + 0x0010UL)

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

#define SysTick       ((SysTick_TypeDef *) SysTick_BASE)

volatile uint32_t msTicks = 0;

void SysTick_Handler(void) {
    msTicks++;
}

void SystemInit(void) {
    // Để trống
}

int main(void) {
    // 1. Bật xung nhịp cho GPIOA (Bit 2 tương ứng IOPAEN)
    RCC_APB2ENR |= (1UL << 2);

    // 2. Cấu hình các chân PA0, PA1, PA2 làm Output Push-Pull (tốc độ 2MHz)
    // Mỗi chân chiếm 4 bit trong thanh ghi CRL (PA0: bits 0-3, PA1: bits 4-7, PA2: bits 8-11)
    GPIOA_CRL &= ~((0xFUL << 0) | (0xFUL << 4) | (0xFUL << 8));
    GPIOA_CRL |=  ((0x2UL << 0) | (0x2UL << 4) | (0x2UL << 8));

    // 3. Cấu hình SysTick ngắt mỗi 1ms (Xung 8MHz)
    SysTick->LOAD = 8000UL - 1UL;
    SysTick->VAL  = 0UL;
    SysTick->CTRL = (1UL << 2) | (1UL << 1) | (1UL << 0);

    uint32_t last_time1 = 0;
    uint32_t last_time2 = 0;
    uint32_t last_time3 = 0;

    while (1) {
        uint32_t current_time = msTicks;

        /* LED 1 (PA0): Nhấp nháy tần số nhanh (mỗi 200ms) */
        if ((current_time - last_time1) >= 200) {
            last_time1 = current_time;
            GPIOA_ODR ^= (1UL << 0);
        }

        /* LED 2 (PA1): Nhấp nháy tần số trung bình (mỗi 500ms) */
        if ((current_time - last_time2) >= 500) {
            last_time2 = current_time;
            GPIOA_ODR ^= (1UL << 1);
        }

        /* LED 3 (PA2): Nhấp nháy tần số chậm (mỗi 1000ms - 1 giây) */
        if ((current_time - last_time3) >= 1000) {
            last_time3 = current_time;
            GPIOA_ODR ^= (1UL << 2);
        }
    }
}
