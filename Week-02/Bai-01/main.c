#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"

#define BUFFER_SIZE 100

volatile char buffer[BUFFER_SIZE];
volatile uint8_t index = 0;

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

    /* PA9 = TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* PA10 = RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* UART: 115200, 8 bit, 1 stop, không parity */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);

    /* Bật ngắt nhận dữ liệu */
    USART_ITConfig(
        USART1,
        USART_IT_RXNE,
        ENABLE
    );

    NVIC_EnableIRQ(USART1_IRQn);

    USART_Cmd(USART1, ENABLE);
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

    USART_SendData(USART1, c);
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

/* Gửi nội dung buffer */
void SendBuffer(void)
{
    uint8_t i = 0;

    /* Mã lớp + mã nhóm */
    UART_SendString("HTN-0314: ");

    /* Gửi nội dung đã nhận */
    while (buffer[i] != '\0')
    {
        UART_SendChar(buffer[i]);
        i++;
    }

    /* Xuống dòng */
    UART_SendString("\n\r");
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

        /* Đọc dữ liệu nhận được */
        c = USART_ReceiveData(USART1);

        /* Gặp ! thì kết thúc bản tin */
        if (c == '!')
        {
            buffer[index] = '\0';

            /* Gửi lại PC */
            SendBuffer();

            /* Nhận bản tin mới */
            index = 0;
        }
        else
        {
            /* Lưu vào buffer */
            if (index < BUFFER_SIZE - 1)
            {
                buffer[index] = c;
                index++;
            }
        }
    }
}

/* ================= MAIN ================= */

int main(void)
{
    /* Khởi tạo UART */
    UART1_Init();

    /* Thông báo khi RESET */
    UART_SendString("Ready!\r\n");

    while (1)
    {
    }
}
