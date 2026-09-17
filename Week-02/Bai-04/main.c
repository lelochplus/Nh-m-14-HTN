#include "stm32f10x.h"

void SystemInit(void) {
    // Khởi tạo hệ thống cơ bản
}

void TIM2_PWM_Init(void) {
    // 1. Bật cấp nguồn Clock cho GPIOA và TIM2
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. Cấu hình chân PA0, PA1, PA2, PA3 thành Alternate Function Push-Pull (50MHz)
    GPIOA->CRL &= 0xFFFF0000;
    GPIOA->CRL |= 0x0000AAAA; 

    // 3. Cấu hình Timer 2 tạo tần số 1kHz (Clock 72MHz)[cite: 1]
    TIM2->PSC = 71;    // 72MHz / (71 + 1) = 1MHz
    TIM2->ARR = 999;   // 1MHz / 1000 = 1kHz

    // 4. Cấu hình PWM Mode 1 cho 4 kênh
    TIM2->CCMR1 |= (6 << 4) | (1 << 3) | (6 << 12) | (1 << 11);
    TIM2->CCMR2 |= (6 << 4) | (1 << 3) | (6 << 12) | (1 << 11);

    // 5. Cho phép xuất xung ra chân
    TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;

    // 6. Gán độ rộng xung (Duty Cycle) đúng chuẩn đề bài: 10%, 30%, 50%, 70%[cite: 1]
    TIM2->CCR1 = 100; // 10%[cite: 1]
    TIM2->CCR2 = 300; // 30%[cite: 1]
    TIM2->CCR3 = 500; // 50%[cite: 1]
    TIM2->CCR4 = 700; // 70%[cite: 1]

    // 7. Kích hoạt Timer
    TIM2->CR1 |= TIM_CR1_CEN;
}

int main(void) {
    TIM2_PWM_Init();

    while (1) {
        // Vòng lặp chờ vô hạn vì phần cứng tự phát PWM ra các chân PA0-PA3
    }
}
