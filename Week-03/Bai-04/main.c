#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#define ADC_BUFFER_SIZE 100

volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

/* =====================================================
   UART1
   PA9  = TX
   PA10 = RX
   115200 8N1
   ===================================================== */

void UART1_Init(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef uart;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_USART1,
        ENABLE
    );

    /* PA9 - TX */
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    /* PA10 - RX */
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    uart.USART_BaudRate = 115200;
    uart.USART_WordLength = USART_WordLength_8b;
    uart.USART_StopBits = USART_StopBits_1;
    uart.USART_Parity = USART_Parity_No;
    uart.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    uart.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &uart);

    USART_Cmd(USART1, ENABLE);
}

/* Gửi 1 ký tự */
void UART_SendChar(char c)
{
    USART_SendData(USART1, c);

    while (USART_GetFlagStatus(
        USART1,
        USART_FLAG_TXE) == RESET);
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

/* Gửi số */
void UART_SendNumber(uint16_t number)
{
    char buffer[6];
    int i = 0;

    if (number == 0)
    {
        UART_SendChar('0');
        return;
    }

    while (number > 0)
    {
        buffer[i++] =
            '0' + (number % 10);

        number /= 10;
    }

    while (i > 0)
    {
        UART_SendChar(buffer[--i]);
    }
}

/* =====================================================
   ADC1
   PA0 = ADC1_IN0
   LM35 OUT -> PA0
   ===================================================== */

void ADC1_Init(void)
{
    GPIO_InitTypeDef gpio;
    ADC_InitTypeDef adc;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_ADC1,
        ENABLE
    );

    /* PA0 = Analog Input */
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_AIN;

    GPIO_Init(GPIOA, &gpio);

    /*
       ADC clock:
       72 MHz / 6 = 12 MHz
    */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    adc.ADC_Mode =
        ADC_Mode_Independent;

    adc.ADC_ScanConvMode =
        DISABLE;

    /*
       Không dùng Continuous.
       Mỗi lần Timer gọi sẽ lấy 1 mẫu.
    */
    adc.ADC_ContinuousConvMode =
        DISABLE;

    /*
       Không dùng trigger ngoài.
       Timer sẽ gọi ADC bằng software.
    */
    adc.ADC_ExternalTrigConv =
        ADC_ExternalTrigConv_None;

    adc.ADC_DataAlign =
        ADC_DataAlign_Right;

    adc.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &adc);

    /* Channel 0 = PA0 */
    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_0,
        1,
        ADC_SampleTime_55Cycles5
    );

    /* Cho phép DMA */
    ADC_DMACmd(
        ADC1,
        ENABLE
    );

    ADC_Cmd(
        ADC1,
        ENABLE
    );

    /* Calibration */
    ADC_ResetCalibration(ADC1);

    while (
        ADC_GetResetCalibrationStatus(ADC1)
    );

    ADC_StartCalibration(ADC1);

    while (
        ADC_GetCalibrationStatus(ADC1)
    );
}

/* =====================================================
   DMA1 CHANNEL 1
   ADC1 -> RAM
   ===================================================== */

void DMA_ADC_Init(void)
{
    DMA_InitTypeDef dma;
    NVIC_InitTypeDef nvic;

    RCC_AHBPeriphClockCmd(
        RCC_AHBPeriph_DMA1,
        ENABLE
    );

    DMA_DeInit(
        DMA1_Channel1
    );

    /* Địa chỉ ADC Data Register */
    dma.DMA_PeripheralBaseAddr =
        (uint32_t)&ADC1->DR;

    /* Địa chỉ buffer RAM */
    dma.DMA_MemoryBaseAddr =
        (uint32_t)adc_buffer;

    /* ADC -> RAM */
    dma.DMA_DIR =
        DMA_DIR_PeripheralSRC;

    /* 100 mẫu */
    dma.DMA_BufferSize =
        ADC_BUFFER_SIZE;

    /* ADC address không tăng */
    dma.DMA_PeripheralInc =
        DMA_PeripheralInc_Disable;

    /* RAM address tăng */
    dma.DMA_MemoryInc =
        DMA_MemoryInc_Enable;

    /* ADC 12 bit -> HalfWord */
    dma.DMA_PeripheralDataSize =
        DMA_PeripheralDataSize_HalfWord;

    dma.DMA_MemoryDataSize =
        DMA_MemoryDataSize_HalfWord;

    /* Chạy vòng */
    dma.DMA_Mode =
        DMA_Mode_Circular;

    dma.DMA_Priority =
        DMA_Priority_High;

    dma.DMA_M2M =
        DMA_M2M_Disable;

    DMA_Init(
        DMA1_Channel1,
        &dma
    );

    /* Ngắt Half Transfer + Transfer Complete */
    DMA_ITConfig(
        DMA1_Channel1,
        DMA_IT_HT | DMA_IT_TC,
        ENABLE
    );

    /* Cấu hình ngắt DMA */
    nvic.NVIC_IRQChannel =
        DMA1_Channel1_IRQn;

    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic);

    /* Bật DMA */
    DMA_Cmd(
        DMA1_Channel1,
        ENABLE
    );
}

/* =====================================================
   TIMER2
   100 Hz = 100 lần/giây
   ===================================================== */

void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef tim;
    NVIC_InitTypeDef nvic;

    RCC_APB1PeriphClockCmd(
        RCC_APB1Periph_TIM2,
        ENABLE
    );

    /*
       Clock Timer = 72 MHz

       72 MHz / 7200 = 10 kHz

       10 kHz / 100 = 100 Hz
    */

    tim.TIM_Prescaler = 7199;

    tim.TIM_CounterMode =
        TIM_CounterMode_Up;

    tim.TIM_Period = 99;

    tim.TIM_ClockDivision =
        TIM_CKD_DIV1;

    tim.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(
        TIM2,
        &tim
    );

    /* Cho phép ngắt mỗi 10 ms */
    TIM_ITConfig(
        TIM2,
        TIM_IT_Update,
        ENABLE
    );

    /* Cấu hình NVIC */
    nvic.NVIC_IRQChannel =
        TIM2_IRQn;

    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&nvic);

    /* Bật Timer */
    TIM_Cmd(
        TIM2,
        ENABLE
    );
}

/* =====================================================
   TIMER2 INTERRUPT
   Mỗi 10 ms:
   Timer -> ADC -> DMA -> RAM
   ===================================================== */

void TIM2_IRQHandler(void)
{
    if (
        TIM_GetITStatus(
            TIM2,
            TIM_IT_Update
        ) != RESET
    )
    {
        /* Xóa cờ ngắt */
        TIM_ClearITPendingBit(
            TIM2,
            TIM_IT_Update
        );

        /*
           Bắt đầu một lần chuyển đổi ADC.
           Kết quả tự động được DMA
           đưa vào adc_buffer[].
        */
        ADC_SoftwareStartConvCmd(
            ADC1,
            ENABLE
        );
    }
}

/* =====================================================
   Gửi 1 mẫu ADC + nhiệt độ
   ===================================================== */

void Send_ADC_Data(
    uint16_t index
)
{
    uint16_t adc;
    uint32_t voltage;
    uint32_t temperature;

    adc = adc_buffer[index];

    /*
       ADC:
       0     -> 0 mV
       4095  -> 3300 mV
    */

    voltage =
        ((uint32_t)adc * 3300) / 4095;

    /*
       LM35:
       10 mV = 1°C
    */

    temperature =
        voltage / 10;

    UART_SendString(
        "ADC = "
    );

    UART_SendNumber(
        adc
    );

    UART_SendString(
        " | Voltage = "
    );

    UART_SendNumber(
        (uint16_t)voltage
    );

    UART_SendString(
        " mV | Temperature = "
    );

    UART_SendNumber(
        (uint16_t)temperature
    );

    UART_SendString(
        " C\r\n"
    );
}

/* =====================================================
   DMA1 CHANNEL1 INTERRUPT
   ===================================================== */

void DMA1_Channel1_IRQHandler(void)
{
    /*
       50 mẫu đầu tiên đã đầy
    */
    if (
        DMA_GetITStatus(
            DMA1_IT_HT1
        ) != RESET
    )
    {
        DMA_ClearITPendingBit(
            DMA1_IT_HT1
        );

        /*
           Gửi mẫu đầu tiên
           của nửa buffer.
        */
        Send_ADC_Data(0);
    }

    /*
       100 mẫu đã đầy
    */
    if (
        DMA_GetITStatus(
            DMA1_IT_TC1
        ) != RESET
    )
    {
        DMA_ClearITPendingBit(
            DMA1_IT_TC1
        );

        /*
           Gửi mẫu đầu tiên
           của nửa thứ hai.
        */
        Send_ADC_Data(50);
    }
}

/* =====================================================
   MAIN
   ===================================================== */

int main(void)
{
    /* UART */
    UART1_Init();

    UART_SendString(
        "\r\n"
        "Bai 04 ADC + DMA + TIMER\r\n"
    );

    UART_SendString(
        "LM35 READY\r\n"
    );

    /* ADC */
    ADC1_Init();

    /* DMA */
    DMA_ADC_Init();

    /* Timer 100 Hz */
    TIM2_Init();

    while (1)
    {
        /* Không cần xử lý trong main */
    }
}
