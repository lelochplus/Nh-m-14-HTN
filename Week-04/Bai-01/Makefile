CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -mcpu=cortex-m3 -mthumb \
         -O0 -Wall \
         -ffreestanding \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER

INCLUDES = -I. \
           -IFreeRTOS \
           -IFreeRTOS-Kernel/include \
           -IFreeRTOS/portable/GCC/ARM_CM3 \
           -ILibraries/CMSIS/CM3/CoreSupport \
           -ILibraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
           -ILibraries/STM32F10x_StdPeriph_Driver/inc

LDSCRIPT = linker.ld

SRC = main.c \
      FreeRTOS/tasks.c \
      FreeRTOS/queue.c \
      FreeRTOS/list.c \
      FreeRTOS/timers.c \
      FreeRTOS/event_groups.c \
      FreeRTOS/stream_buffer.c \
      FreeRTOS/croutine.c \
      FreeRTOS/portable/MemMang/heap_4.c \
      FreeRTOS/portable/GCC/ARM_CM3/port.c \
      Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
      Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c

STARTUP = startup_stm32f10x_md.s

OBJ = $(SRC:.c=.o)

TARGET = main


all: $(TARGET).bin


$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) \
	-T $(LDSCRIPT) \
	$(STARTUP) $(OBJ) \
	-nostdlib \
	-Wl,--gc-sections \
	-o $(TARGET).elf


$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin


%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


clean:
	rm -f $(OBJ)
	rm -f $(TARGET).elf
	rm -f $(TARGET).bin


flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000
