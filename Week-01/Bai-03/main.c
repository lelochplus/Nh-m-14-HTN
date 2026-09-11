#include <stdint.h>



#define RCC_BASE        0x40021000UL
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define AFIOEN          (1U << 0)
#define IOPAEN          (1U << 2)
#define IOPBEN          (1U << 3)


#define AFIO_BASE       0x40010000UL
#define AFIO_MAPR       (*(volatile uint32_t *)(AFIO_BASE + 0x04))


#define GPIOA_BASE      0x40010800UL

#define GPIOA_CRL       (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_CRH       (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))



#define GPIOB_BASE      0x40010C00UL

#define GPIOB_CRL       (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_ODR       (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))


void gpio_init(void)
{

    RCC_APB2ENR |= IOPAEN | IOPBEN | AFIOEN;



    AFIO_MAPR &= ~(7U << 24);
    AFIO_MAPR |=  (2U << 24);


    GPIOA_CRL = 0x88888888U;

    GPIOA_ODR |= 0x00FFU;



    GPIOA_CRH &= ~(
        (0xFU << 0)  |  
        (0xFU << 4)  |   
        (0xFU << 8)  |  
        (0xFU << 12) |   
        (0xFU << 16) |   
        (0xFU << 28)     
    );


    GPIOA_CRH |= (
        (0x1U << 0)  |  
        (0x1U << 4)  |  
        (0x1U << 8)  |   
        (0x1U << 12) |  
        (0x1U << 16) |  
        (0x1U << 28)     
    );



    GPIOB_CRL &= ~(
        (0xFU << 12) |   
        (0xFU << 16)     
    );

    GPIOB_CRL |= (
        (0x1U << 12) |   
        (0x1U << 16)     
    );



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



int main(void)
{
    uint32_t input;


    gpio_init();



    while (1)
    {
        input = GPIOA_IDR & 0x00FFU;



        input = (~input) & 0x00FFU;



        if (input & (1U << 0))
            GPIOA_ODR |= (1U << 8);
        else
            GPIOA_ODR &= ~(1U << 8);



        if (input & (1U << 1))
            GPIOA_ODR |= (1U << 9);
        else
            GPIOA_ODR &= ~(1U << 9);



        if (input & (1U << 2))
            GPIOA_ODR |= (1U << 10);
        else
            GPIOA_ODR &= ~(1U << 10);


        if (input & (1U << 3))
            GPIOA_ODR |= (1U << 11);
        else
            GPIOA_ODR &= ~(1U << 11);



        if (input & (1U << 4))
            GPIOA_ODR |= (1U << 12);
        else
            GPIOA_ODR &= ~(1U << 12);



        if (input & (1U << 5))
            GPIOA_ODR |= (1U << 15);
        else
            GPIOA_ODR &= ~(1U << 15);



        if (input & (1U << 6))
            GPIOB_ODR |= (1U << 3);
        else
            GPIOB_ODR &= ~(1U << 3);



        if (input & (1U << 7))
            GPIOB_ODR |= (1U << 4);
        else
            GPIOB_ODR &= ~(1U << 4);
    }
}
