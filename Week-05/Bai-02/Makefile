CC = arm-none-eabi-gcc

CFLAGS = -mcpu=cortex-m3 \
         -mthumb \
         -O0 \
         -Wall \
         -ffreestanding \
         -DSTM32F10X_MD \
         -DUSE_STDPERIPH_DRIVER \
         -I. \
         -IFreeRTOS/include \
         -IFreeRTOS/portable/GCC/ARM_CM3 \
         -ILibraries/CMSIS/CM3/CoreSupport \
         -ILibraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
         -ILibraries/STM32F10x_StdPeriph_Driver/inc

LDFLAGS = -T linker.ld \
          -nostdlib \
          -Wl,--gc-sections

OBJS = main.o \
       FreeRTOS/tasks.o \
       FreeRTOS/queue.o \
       FreeRTOS/list.o \
       FreeRTOS/timers.o \
       FreeRTOS/event_groups.o \
       FreeRTOS/stream_buffer.o \
       FreeRTOS/croutine.o \
       FreeRTOS/portable/MemMang/heap_4.o \
       FreeRTOS/portable/GCC/ARM_CM3/port.o \
       Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.o \
       Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.o \
       Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.o \
       Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.o \
       Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.o \
       Libraries/STM32F10x_StdPeriph_Driver/src/misc.o

all: main.elf main.bin

main.elf: $(OBJS)
	$(CC) $(CFLAGS) \
	startup_stm32f10x_md.s \
	$(OBJS) \
	$(LDFLAGS) \
	-o main.elf

main.bin: main.elf
	arm-none-eabi-objcopy -O binary main.elf main.bin

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

FreeRTOS/tasks.o: FreeRTOS/tasks.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/queue.o: FreeRTOS/queue.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/list.o: FreeRTOS/list.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/timers.o: FreeRTOS/timers.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/event_groups.o: FreeRTOS/event_groups.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/stream_buffer.o: FreeRTOS/stream_buffer.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/croutine.o: FreeRTOS/croutine.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/portable/MemMang/heap_4.o: FreeRTOS/portable/MemMang/heap_4.c
	$(CC) $(CFLAGS) -c $< -o $@

FreeRTOS/portable/GCC/ARM_CM3/port.o: FreeRTOS/portable/GCC/ARM_CM3/port.c
	$(CC) $(CFLAGS) -c $< -o $@

Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.o: \
Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
	$(CC) $(CFLAGS) -c $< -o $@

Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.o: \
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c
	$(CC) $(CFLAGS) -c $< -o $@

Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.o: \
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c
	$(CC) $(CFLAGS) -c $< -o $@

Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.o: \
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c
	$(CC) $(CFLAGS) -c $< -o $@

Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.o: \
Libraries/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c
	$(CC) $(CFLAGS) -c $< -o $@

Libraries/STM32F10x_StdPeriph_Driver/src/misc.o: \
Libraries/STM32F10x_StdPeriph_Driver/src/misc.c
	$(CC) $(CFLAGS) -c $< -o $@

# ================= FLASH =================

flash: main.bin
	st-flash write main.bin 0x08000000

# ================= CLEAN =================

clean:
	rm -f main.o main.elf main.bin
	rm -f FreeRTOS/*.o
	rm -f FreeRTOS/portable/MemMang/*.o
	rm -f FreeRTOS/portable/GCC/ARM_CM3/*.o
	rm -f Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/*.o
	rm -f Libraries/STM32F10x_StdPeriph_Driver/src/*.o
