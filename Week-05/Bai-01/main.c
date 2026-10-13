#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define LED_PORT        GPIOA
#define LED1_PIN        GPIO_Pin_0
#define LED2_PIN        GPIO_Pin_1
#define LED3_PIN        GPIO_Pin_2

#define BTN_PORT        GPIOA
#define BTN_CHANGE      GPIO_Pin_3
#define BTN_UP          GPIO_Pin_4
#define BTN_DOWN        GPIO_Pin_5

#define BRIGHT_MIN      0
#define BRIGHT_MAX      100
#define BRIGHT_STEP     10
#define DEBOUNCE_MS     150

typedef struct
{
    uint8_t led;
    uint8_t brightness;
} LED_Message;

QueueHandle_t ledQueue[3];


/* ================= PWM ================= */

void PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    GPIO_InitStructure.GPIO_Pin =
        LED1_PIN | LED2_PIN | LED3_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);

    /* 72 MHz / 72 = 1 MHz
       1 MHz / 1000 = 1 kHz */
    TIM2->PSC = 71;
    TIM2->ARR = 999;

    /* PWM mode 1 */
    TIM2->CCMR1 =
        (6 << 4)  | (1 << 3) |
        (6 << 12) | (1 << 11);

    TIM2->CCMR2 = (6 << 4) | (1 << 3);

    TIM2->CCER =
        TIM_CCER_CC1E |
        TIM_CCER_CC2E |
        TIM_CCER_CC3E;

    /* 50% ban đầu */
    TIM2->CCR1 = 500;
    TIM2->CCR2 = 500;
    TIM2->CCR3 = 500;

    TIM2->CR1 |= TIM_CR1_ARPE;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->CR1 |= TIM_CR1_CEN;
}


/* ================= BUTTON ================= */

void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin =
        BTN_CHANGE | BTN_UP | BTN_DOWN;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(BTN_PORT, &GPIO_InitStructure);
}


/* ================= LED TASK ================= */

void LED_Task(void *param)
{
    uint8_t led = (uint8_t)param;
    LED_Message msg;

    while (1)
    {
        if (xQueueReceive(ledQueue[led], &msg, portMAX_DELAY))
        {
            if (msg.led == led)
            {
                uint16_t duty =
                    (msg.brightness * 999) / 100;

                if (led == 0)
                    TIM2->CCR1 = duty;
                else if (led == 1)
                    TIM2->CCR2 = duty;
                else
                    TIM2->CCR3 = duty;
            }
        }
    }
}


/* ================= BUTTON TASK ================= */

void Button_Task(void *param)
{
    uint8_t selected_led = 0;
    uint8_t brightness[3] = {50, 50, 50};
    LED_Message msg;

    while (1)
    {
        /* Đổi LED */
        if (GPIO_ReadInputDataBit(BTN_PORT, BTN_CHANGE) == Bit_RESET)
        {
            selected_led = (selected_led + 1) % 3;

            msg.led = selected_led;
            msg.brightness = brightness[selected_led];

            for (uint8_t i = 0; i < 3; i++)
                xQueueSend(ledQueue[i], &msg, 0);

            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
        }

        /* Tăng sáng */
        if (GPIO_ReadInputDataBit(BTN_PORT, BTN_UP) == Bit_RESET)
        {
            if (brightness[selected_led] < BRIGHT_MAX)
                brightness[selected_led] += BRIGHT_STEP;

            msg.led = selected_led;
            msg.brightness = brightness[selected_led];

            for (uint8_t i = 0; i < 3; i++)
                xQueueSend(ledQueue[i], &msg, 0);

            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
        }

        /* Giảm sáng */
        if (GPIO_ReadInputDataBit(BTN_PORT, BTN_DOWN) == Bit_RESET)
        {
            if (brightness[selected_led] > BRIGHT_MIN)
                brightness[selected_led] -= BRIGHT_STEP;

            msg.led = selected_led;
            msg.brightness = brightness[selected_led];

            for (uint8_t i = 0; i < 3; i++)
                xQueueSend(ledQueue[i], &msg, 0);

            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


/* ================= MAIN ================= */

int main(void)
{
    PWM_Init();
    Button_Init();

    /* Tạo 3 Queue */
    for (uint8_t i = 0; i < 3; i++)
    {
        ledQueue[i] = xQueueCreate(5, sizeof(LED_Message));

        if (ledQueue[i] == NULL)
            while (1);
    }

    /* 3 LED task */
    xTaskCreate(LED_Task, "LED1", 128,
                (void *)0, 1, NULL);

    xTaskCreate(LED_Task, "LED2", 128,
                (void *)1, 1, NULL);

    xTaskCreate(LED_Task, "LED3", 128,
                (void *)2, 1, NULL);

    /* Button task */
    xTaskCreate(Button_Task, "BUTTON", 128,
                NULL, 2, NULL);

    vTaskStartScheduler();

    while (1);
}
