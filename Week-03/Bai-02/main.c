#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"

#define assert_param(expr) ((void)0)

#define CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

/* Font số 0 -> 9, mỗi số 5 cột */
const uint8_t number[10][5] =
{
    /* 0 */
    {0x3E, 0x51, 0x49, 0x45, 0x3E},

    /* 1 */
    {0x00, 0x42, 0x7F, 0x40, 0x00},

    /* 2 */
    {0x42, 0x61, 0x51, 0x49, 0x46},

    /* 3 */
    {0x21, 0x41, 0x45, 0x4B, 0x31},

    /* 4 */
    {0x18, 0x14, 0x12, 0x7F, 0x10},

    /* 5 */
    {0x27, 0x45, 0x45, 0x45, 0x39},

    /* 6 */
    {0x3C, 0x4A, 0x49, 0x49, 0x30},

    /* 7 */
    {0x01, 0x71, 0x09, 0x05, 0x03},

    /* 8 */
    {0x36, 0x49, 0x49, 0x49, 0x36},

    /* 9 */
    {0x06, 0x49, 0x49, 0x29, 0x1E}
};


void delay_ms(uint32_t ms)
{
    uint32_t i;

    while(ms--)
    {
        for(i = 0; i < 8000; i++);
    }
}


/* Gửi 1 byte SPI */
void SPI_SendByte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    SPI_I2S_SendData(SPI1, data);

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}


/* Ghi vào MAX7219 */
void MAX7219_Write(uint8_t address, uint8_t data)
{
    CS_LOW();

    SPI_SendByte(address);
    SPI_SendByte(data);

    CS_HIGH();
}


/* Khởi tạo MAX7219 */
void MAX7219_Init(void)
{
    MAX7219_Write(0x09, 0x00);   // Không decode
    MAX7219_Write(0x0A, 0x05);   // Độ sáng
    MAX7219_Write(0x0B, 0x07);   // 8 hàng
    MAX7219_Write(0x0C, 0x01);   // Bật màn hình
    MAX7219_Write(0x0F, 0x00);   // Tắt test
}


/* Xóa LED */
void MAX7219_Clear(void)
{
    uint8_t i;

    for(i = 1; i <= 8; i++)
    {
        MAX7219_Write(i, 0x00);
    }
}


/* Hiển thị 8 cột */
void DisplayColumns(uint8_t *data)
{
    uint8_t i;

    for(i = 0; i < 8; i++)
    {
        MAX7219_Write(i + 1, data[i]);
    }
}


/* Cho 1 số chạy ngang */
void ScrollNumber(uint8_t num)
{
    uint8_t screen[8];
    uint8_t text[13];

    uint8_t i;
    uint8_t j;

    /*
       4 cột trống đầu
       5 cột số
       4 cột trống cuối
    */

    for(i = 0; i < 4; i++)
        text[i] = 0x00;

    for(i = 0; i < 5; i++)
        text[i + 4] = number[num][i];

    for(i = 9; i < 13; i++)
        text[i] = 0x00;


    /*
       Trượt cửa sổ 8 cột
       từ phải sang trái
    */
    for(i = 0; i <= 13; i++)
    {
        for(j = 0; j < 8; j++)
        {
            if(i + j < 13)
                screen[j] = text[i + j];
            else
                screen[j] = 0x00;
        }

        DisplayColumns(screen);

        delay_ms(150);
    }
}


/* Khởi tạo SPI1 */
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(
        RCC_APB2Periph_GPIOA |
        RCC_APB2Periph_SPI1,
        ENABLE
    );

    /* PA5 = SCK, PA7 = MOSI */
    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_5 |
        GPIO_Pin_7;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);


    /* PA4 = CS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(GPIOA, &GPIO_InitStructure);

    CS_HIGH();


    /* SPI mode 0 */
    SPI_InitStructure.SPI_Direction =
        SPI_Direction_2Lines_FullDuplex;

    SPI_InitStructure.SPI_Mode =
        SPI_Mode_Master;

    SPI_InitStructure.SPI_DataSize =
        SPI_DataSize_8b;

    SPI_InitStructure.SPI_CPOL =
        SPI_CPOL_Low;

    SPI_InitStructure.SPI_CPHA =
        SPI_CPHA_1Edge;

    SPI_InitStructure.SPI_NSS =
        SPI_NSS_Soft;

    SPI_InitStructure.SPI_BaudRatePrescaler =
        SPI_BaudRatePrescaler_16;

    SPI_InitStructure.SPI_FirstBit =
        SPI_FirstBit_MSB;

    SPI_InitStructure.SPI_CRCPolynomial = 7;

    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);
}


int main(void)
{
    uint8_t num;

    SystemInit();

    SPI1_Init();

    MAX7219_Init();

    MAX7219_Clear();

    while(1)
    {
        for(num = 0; num <= 9; num++)
        {
            ScrollNumber(num);

            delay_ms(100);
        }
    }
}
