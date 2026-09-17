#ifndef __STM32F10x_H
#define __STM32F10x_H

#include <stdint.h>

#define __IO volatile

#define PERIPH_BASE         (0x40000000UL)
#define APB1PERIPH_BASE     (PERIPH_BASE)
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x00010000UL)
#define AHBPERIPH_BASE      (PERIPH_BASE + 0x00020000UL)

#define RCC_BASE            (AHBPERIPH_BASE + 0x1000UL)
#define GPIOA_BASE          (APB2PERIPH_BASE + 0x0800UL)
#define TIM2_BASE           (APB1PERIPH_BASE + 0x0000UL)

typedef struct {
    __IO uint32_t CRL;
    __IO uint32_t CRH;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t BRR;
    __IO uint32_t LCKR;
} GPIO_TypeDef;

typedef struct {
    __IO uint32_t CR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t APB2RSTR;
    __IO uint32_t APB1RSTR;
    __IO uint32_t AHBENR;
    __IO uint32_t APB2ENR;
    __IO uint32_t APB1ENR;
} RCC_TypeDef;

typedef struct {
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t SMCR;
    __IO uint32_t DIER;
    __IO uint32_t SR;
    __IO uint32_t EGR;
    __IO uint32_t CCMR1;
    __IO uint32_t CCMR2;
    __IO uint32_t CCER;
    __IO uint32_t CNT;
    __IO uint32_t PSC;
    __IO uint32_t ARR;
    __IO uint32_t RESERVED0;
    __IO uint32_t CCR1;
    __IO uint32_t CCR2;
    __IO uint32_t CCR3;
    __IO uint32_t CCR4;
} TIM_TypeDef;

#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define TIM2                ((TIM_TypeDef *) TIM2_BASE)

#define RCC_APB2ENR_IOPAEN  (1UL << 2)
#define RCC_APB1ENR_TIM2EN  (1UL << 0)
#define TIM_CCER_CC1E       (1UL << 0)
#define TIM_CCER_CC2E       (1UL << 4)
#define TIM_CCER_CC3E       (1UL << 8)
#define TIM_CCER_CC4E       (1UL << 12)
#define TIM_CR1_CEN         (1UL << 0)

void SystemInit(void);

#endif
