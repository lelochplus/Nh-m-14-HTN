TARGET = stm32_systick

CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

CFLAGS = -O0 -g3 -Wall -mthumb -mcpu=cortex-m3 -ffreestanding
LDFLAGS = -Tstm32_flash.ld -nostdlib -lc -lgcc

all: $(TARGET).bin

$(TARGET).elf: main.c startup.s
	$(CC) $(CFLAGS) main.c startup.s $(LDFLAGS) -o $(TARGET).elf

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin

clean:
	rm -f *.elf *.bin
flash: all
	st-flash write $(TARGET).bin 0x08000000
