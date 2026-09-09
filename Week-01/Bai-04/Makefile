CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS = -mcpu=cortex-m3 -mthumb -O0 -Wall -ffreestanding
LDFLAGS = -T linker.ld -nostdlib -Wl,--gc-sections

TARGET = main

all: $(TARGET).bin

$(TARGET).elf: main.c startup.s linker.ld
	$(CC) $(CFLAGS) $(LDFLAGS) main.c startup.s -o $(TARGET).elf

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	$(SIZE) $(TARGET).elf

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f $(TARGET).elf $(TARGET).bin
