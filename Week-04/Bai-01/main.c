#include "stm32f10x.h"

#include "FreeRTOS.h"
#include "task.h"

#define LED1_PIN    GPIO_Pin_0
#define LED2_PIN    GPIO_Pin_1
#define LED3_PIN    GPIO_Pin_2

#define LED_PORT    GPIOA

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin =
        LED1_PIN | LED2_PIN | LED3_PIN;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(LED_PORT, &GPIO_InitStructure);

    GPIO_ResetBits(LED_PORT,
                   LED1_PIN | LED2_PIN | LED3_PIN);
}


void LED_Blink(uint16_t pin, float frequency)
{
    uint32_t delay_ms;

    delay_ms = (uint32_t)(1000.0f / (2.0f * frequency));

    while (1)
    {
        GPIO_SetBits(LED_PORT, pin);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        GPIO_ResetBits(LED_PORT, pin);
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
    }
}

void LED1_Task(void *pvParameters)
{
    LED_Blink(LED1_PIN, 0.1f);
}

void LED2_Task(void *pvParameters)
{
    LED_Blink(LED2_PIN, 1.0f);
}

void LED3_Task(void *pvParameters)
{
    LED_Blink(LED3_PIN, 10.0f);
}

int main(void)
{
    LED_Init();

    xTaskCreate(
        LED1_Task,
        "LED1",
        128,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        LED2_Task,
        "LED2",
        128,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        LED3_Task,
        "LED3",
        128,
        NULL,
        1,
        NULL
    );

    vTaskStartScheduler();

    while (1)
    {
    }
}
