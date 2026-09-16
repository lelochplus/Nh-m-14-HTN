#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"

#define BUFFER_SIZE 50

volatile char buffer[BUFFER_SIZE];
volatile uint8_t index = 0;

volatile uint8_t led_state = 0;
volatile uint8_t pwm_percent = 50;


/* ================= PWM PA0 - TIM2_CH1 ================= */

void TIM2_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* Bật clock GPIOA */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA,
        ENABLE
    );

    /* Bật clock TIM2 */
    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_TIM2,
        ENABLE
    );

    /* PA0 = TIM2_CH1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* Cấu hình Timer */

    /*
       Clock STM32 = 72 MHz

       Prescaler = 35
       Timer clock = 72MHz / 36 = 2MHz

       Period = 999
       PWM = 2MHz / 1000 = 2kHz
    */

    TIM_TimeBaseStructure.TIM_Prescaler = 35;
    TIM_TimeBaseStructure.TIM_CounterMode =
        TIM_CounterMode_Up;

    TIM_TimeBaseStructure.TIM_Period = 999;

    TIM_TimeBaseStructure.TIM_ClockDivision =
        TIM_CKD_DIV1;

    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(
        TIM2,
        &TIM_TimeBaseStructure
    );


    /* Cấu hình PWM Channel 1 */

    TIM_OCInitStructure.TIM_OCMode =
        TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState =
        TIM_OutputState_Enable;

    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OCInitStructure.TIM_OCPolarity =
        TIM_OCPolarity_High;

    TIM_OC1Init(
        TIM2,
        &TIM_OCInitStructure
    );


    /* Cho phép cập nhật giá trị PWM */

    TIM_OC1PreloadConfig(
        TIM2,
        TIM_OCPreload_Enable
    );

    TIM_ARRPreloadConfig(
        TIM2,
        ENABLE
    );


    /* Bật Timer 2 */

    TIM_Cmd(
        TIM2,
        ENABLE
    );
}


/* Đặt độ sáng PWM từ 0 đến 100% */

void PWM_SetPercent(uint8_t percent)
{
    if (percent > 100)
        percent = 100;

    TIM_SetCompare1(
        TIM2,
        percent * 10
    );
}


/* ================= UART1 ================= */

void UART1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    /* Bật clock GPIOA và USART1 */

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );


    /* PA9 = USART1_TX */

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;

    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_AF_PP;

    GPIO_InitStructure.GPIO_Speed =
        GPIO_Speed_50MHz;

    GPIO_Init(
        GPIOA,
        &GPIO_InitStructure
    );


    /* PA10 = USART1_RX */

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_10;

    GPIO_InitStructure.GPIO_Mode =
        GPIO_Mode_IN_FLOATING;

    GPIO_Init(
        GPIOA,
        &GPIO_InitStructure
    );


    /* UART 115200 - 8N1 */

    USART_InitStructure.USART_BaudRate =
        115200;

    USART_InitStructure.USART_WordLength =
        USART_WordLength_8b;

    USART_InitStructure.USART_StopBits =
        USART_StopBits_1;

    USART_InitStructure.USART_Parity =
        USART_Parity_No;

    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Tx |
        USART_Mode_Rx;


    USART_Init(
        USART1,
        &USART_InitStructure
    );


    /* Cho phép ngắt nhận UART */

    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        ENABLE
    );


    /* Cho phép ngắt USART1 */

    NVIC_EnableIRQ(
        USART1_IRQn
    );


    /* Bật USART1 */

    USART_Cmd(
        USART1,
        ENABLE
    );
}


/* Gửi 1 ký tự */

void UART_SendChar(char c)
{
    while (
        USART_GetFlagStatus(
            USART1,
            USART_FLAG_TXE
        ) == RESET
    );

    USART_SendData(
        USART1,
        c
    );
}


/* Gửi chuỗi */

void UART_SendString(const char *str)
{
    while (*str)
    {
        UART_SendChar(*str);
        str++;
    }
}


/* ================= XỬ LÝ LỆNH ================= */

void ProcessCommand(void)
{
    /* ---------- ON! ---------- */

    if (buffer[0] == 'O' &&
        buffer[1] == 'N' &&
        buffer[2] == '\0')
    {
        led_state = 1;

        /* Bật LED theo PWM đã lưu */
        PWM_SetPercent(pwm_percent);

        UART_SendString(
            "LED ON\r\n"
        );
    }


    /* ---------- OFF! ---------- */

    else if (
        buffer[0] == 'O' &&
        buffer[1] == 'F' &&
        buffer[2] == 'F' &&
        buffer[3] == '\0'
    )
    {
        led_state = 0;

        /* Tắt LED */
        PWM_SetPercent(0);

        UART_SendString(
            "LED OFF\r\n"
        );
    }


    /* ---------- Status! ---------- */

    else if (
        buffer[0] == 'S' &&
        buffer[1] == 't' &&
        buffer[2] == 'a' &&
        buffer[3] == 't' &&
        buffer[4] == 'u' &&
        buffer[5] == 's' &&
        buffer[6] == '\0'
    )
    {
        UART_SendString(
            "Status: "
        );

        if (led_state)
        {
            UART_SendString(
                "ON\r\n"
            );
        }
        else
        {
            UART_SendString(
                "OFF\r\n"
            );
        }
    }


    /* ---------- PWM:xx%! ---------- */

    else if (
        buffer[0] == 'P' &&
        buffer[1] == 'W' &&
        buffer[2] == 'M' &&
        buffer[3] == ':'
    )
    {
        uint8_t value = 0;
        uint8_t i = 4;


        /* Đọc số phần trăm */

        while (
            buffer[i] >= '0' &&
            buffer[i] <= '9'
        )
        {
            value =
                value * 10 +
                (buffer[i] - '0');

            i++;
        }


        /* Kiểm tra dấu % */

        if (
            buffer[i] == '%' &&
            buffer[i + 1] == '\0' &&
            value <= 100
        )
        {
            /* Lưu mức PWM */

            pwm_percent = value;


            /*
               Chỉ thay đổi độ sáng
               nếu LED đang ON
            */

            if (led_state)
            {
                PWM_SetPercent(
                    pwm_percent
                );
            }


            UART_SendString(
                "PWM OK\r\n"
            );
        }
    }
}


/* ================= NGẮT UART ================= */

void USART1_IRQHandler(void)
{
    if (
        USART_GetITStatus(
            USART1,
            USART_IT_RXNE
        ) != RESET
    )
    {
        char c;

        c = USART_ReceiveData(
            USART1
        );


        /* Nhận dấu ! = kết thúc lệnh */

        if (c == '!')
        {
            buffer[index] = '\0';

            ProcessCommand();

            index = 0;
        }
        else
        {
            /* Lưu ký tự vào buffer */

            if (
                index <
                BUFFER_SIZE - 1
            )
            {
                buffer[index++] = c;
            }
        }
    }
}


/* ================= MAIN ================= */

int main(void)
{
    /* Khởi tạo PWM PA0 */
    TIM2_PWM_Init();

    /* Khởi tạo UART */
    UART1_Init();


    /* Ban đầu LED OFF */

    led_state = 0;

    PWM_SetPercent(0);


    /* Thông báo UART */

    UART_SendString(
        "Ready!\r\n"
    );


    /* Vòng lặp chính */

    while (1)
    {
    }
}
