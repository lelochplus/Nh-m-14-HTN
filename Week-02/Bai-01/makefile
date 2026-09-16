CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -O0 \
         -Wall \
         -ffreestanding \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER

INCLUDES = -I. \
           -ILibraries/CMSIS/CM3/CoreSupport \
           -ILibraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
           -ILibraries/STM32F10x_StdPeriph_Driver/inc

LDFLAGS = -T linker.ld \
          -nostdlib
SRC = main.c \
      Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c

STARTUP = Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md.s

all: main.bin

main.elf:
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) \
	$(SRC) $(STARTUP) \
	-o main.elf

main.bin: main.elf
	$(OBJCOPY) -O binary main.elf main.bin

clean:
	rm -f main.elf main.bin

flash: main.bin
	st-flash write main.bin 0x08000000
