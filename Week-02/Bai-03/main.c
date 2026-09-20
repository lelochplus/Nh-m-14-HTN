#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_usart.h"

/* ================= UART ================= */

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

    /* UART 115200 8N1 */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;

    USART_InitStructure.USART_Mode =
        USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);

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

/* Gửi số nguyên */
void UART_SendNumber(uint32_t number)
{
    char buffer[10];
    int i = 0;

    if (number == 0)
    {
        UART_SendChar('0');
        return;
    }

    while (number > 0)
    {
        buffer[i] = (number % 10) + '0';
        number /= 10;
        i++;
    }

    while (i > 0)
    {
        i--;
        UART_SendChar(buffer[i]);
    }
}

/* ================= ADC ================= */

void ADC1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* Bật clock GPIOA và ADC1 */
    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_ADC1,
        ENABLE
    );

    /*
     * PCLK2 = 72 MHz
     * ADC clock = 72 / 6 = 12 MHz
     */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    /* PA0 = Analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv =
        ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;

    ADC_Init(ADC1, &ADC_InitStructure);

    /* Channel 0 = PA0 */
    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_0,
        1,
        ADC_SampleTime_239Cycles5
    );

    ADC_Cmd(ADC1, ENABLE);

    /* Reset calibration */
    ADC_ResetCalibration(ADC1);

    while (
        ADC_GetResetCalibrationStatus(ADC1)
    );

    /* Calibration */
    ADC_StartCalibration(ADC1);

    while (
        ADC_GetCalibrationStatus(ADC1)
    );
}

/* Đọc ADC */
uint16_t ADC_Read(void)
{
    ADC_SoftwareStartConvCmd(
        ADC1,
        ENABLE
    );

    while (
        ADC_GetFlagStatus(
            ADC1,
            ADC_FLAG_EOC
        ) == RESET
    );

    return ADC_GetConversionValue(ADC1);
}

/* ================= Delay ================= */

void Delay_ms(uint32_t ms)
{
    uint32_t i;

    while (ms--)
    {
        for (i = 0; i < 8000; i++)
        {
            __NOP();
        }
    }
}

/* ================= MAIN ================= */

int main(void)
{
    uint16_t adc_value;
    uint32_t voltage_mv;
    uint32_t temperature;

    UART1_Init();
    ADC1_Init();

    UART_SendString("LM35 Ready!\r\n");

    while (1)
    {
        /* Đọc ADC */
        adc_value = ADC_Read();

        /*
         * Điện áp mV:
         *
         * V = ADC * 3300 / 4095
         */
        voltage_mv =
            ((uint32_t)adc_value * 3300)
            / 4095;

        /*
         * LM35:
         * 10 mV = 1 độ C
         */
        temperature =
            voltage_mv / 10;

        UART_SendString("ADC = ");
        UART_SendNumber(adc_value);

        UART_SendString(" | Voltage = ");

        UART_SendNumber(
            voltage_mv / 1000
        );

        UART_SendChar('.');

        /* Hiển thị đủ 3 chữ số mV */
        if ((voltage_mv % 1000) < 100)
        {
            UART_SendChar('0');
        }

        if ((voltage_mv % 1000) < 10)
        {
            UART_SendChar('0');
        }

        UART_SendNumber(
            voltage_mv % 1000
        );

        UART_SendString(" V");

        UART_SendString(" | Temperature = ");

        UART_SendNumber(
            temperature
        );

        UART_SendString(" C\r\n");

        Delay_ms(1000);
    }
}
