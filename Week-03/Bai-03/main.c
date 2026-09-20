#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_exti.h"
#include "misc.h"

volatile uint32_t count = 0;
volatile uint8_t send_flag = 0;

char msg[25];

/* UART1: PA9 TX, PA10 RX */
void UART_Init(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1 |
        RCC_APB2Periph_AFIO, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    uart.USART_BaudRate = 115200;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &uart);
    USART_Cmd(USART1, ENABLE);
}

/* DMA1 Channel 4 -> USART1 TX */
void DMA_Init_UART(void)
{
    DMA_InitTypeDef dma;

    RCC_AHBPeriphClockCmd(
        RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);

    dma.DMA_PeripheralBaseAddr =
        (uint32_t)&USART1->DR;

    dma.DMA_MemoryBaseAddr =
        (uint32_t)msg;

    dma.DMA_DIR =
        DMA_DIR_PeripheralDST;

    dma.DMA_BufferSize = 25;

    dma.DMA_PeripheralInc =
        DMA_PeripheralInc_Disable;

    dma.DMA_MemoryInc =
        DMA_MemoryInc_Enable;

    dma.DMA_PeripheralDataSize =
        DMA_PeripheralDataSize_Byte;

    dma.DMA_MemoryDataSize =
        DMA_MemoryDataSize_Byte;

    dma.DMA_Mode =
        DMA_Mode_Normal;

    dma.DMA_Priority =
        DMA_Priority_High;

    dma.DMA_M2M =
        DMA_M2M_Disable;

    DMA_Init(DMA1_Channel4, &dma);

    USART_DMACmd(
        USART1,
        USART_DMAReq_Tx,
        ENABLE);
}

/* PA0: nút nhấn */
void Button_Init(void)
{
    GPIO_InitTypeDef gpio;
    EXTI_InitTypeDef exti;
    NVIC_InitTypeDef nvic;

    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    GPIO_EXTILineConfig(
        GPIO_PortSourceGPIOA,
        GPIO_PinSource0);

    exti.EXTI_Line = EXTI_Line0;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;

    EXTI_Init(&exti);

    nvic.NVIC_IRQChannel = EXTI0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic);
}

/* Ngắt khi nhấn nút */
void EXTI0_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        count++;
        send_flag = 1;

        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/* Chuyển số sang chuỗi */
uint16_t NumberToString(
    uint32_t n,
    char *buf)
{
    char temp[10];
    uint16_t i = 0;
    uint16_t j = 0;

    if (n == 0)
    {
        buf[0] = '0';
        return 1;
    }

    while (n)
    {
        temp[i++] = '0' + n % 10;
        n /= 10;
    }

    while (i)
        buf[j++] = temp[--i];

    return j;
}

/* Tạo chuỗi */
uint16_t MakeMessage(void)
{
    uint16_t i = 0;

    msg[i++] = 'H';
    msg[i++] = 'T';
    msg[i++] = 'N';
    msg[i++] = '-';
    msg[i++] = '0';
    msg[i++] = '3';
    msg[i++] = '1';
    msg[i++] = '4';
    msg[i++] = ':';
    msg[i++] = 'B';
    msg[i++] = 'T';
    msg[i++] = 'N';
    msg[i++] = ':';

    i += NumberToString(count, &msg[i]);

    msg[i++] = '\n';
    msg[i++] = '\r';

    return i;
}

/* Gửi bằng DMA */
void DMA_Send(uint16_t length)
{
    DMA_Cmd(DMA1_Channel4, DISABLE);

    DMA1_Channel4->CMAR =
        (uint32_t)msg;

    DMA1_Channel4->CNDTR =
        length;

    DMA_ClearFlag(DMA1_FLAG_TC4);

    DMA_Cmd(DMA1_Channel4, ENABLE);
}

int main(void)
{
    uint16_t length;

    UART_Init();
    DMA_Init_UART();
    Button_Init();

    while (1)
    {
        if (send_flag)
        {
            send_flag = 0;

            length = MakeMessage();

            DMA_Send(length);
        }
    }
}
