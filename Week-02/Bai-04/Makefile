TARGET = stm32_pwm_demo
PREFIX = arm-none-eabi-

CC      = $(PREFIX)gcc
AS      = $(PREFIX)gcc -x assembler-with-cpp
CP      = $(PREFIX)objcopy
SZ      = $(PREFIX)size
RM      = rm -f

CPU = -mcpu=cortex-m3 -mthumb

CFLAGS  = $(CPU) -O0 -g -Wall -fno-common \
          -DSTM32F10X_MD \
          -ffunction-sections -fdata-sections

# Thêm -T stm32f103.ld để linker phân bổ đúng vùng nhớ
LDFLAGS = $(CPU) -specs=nosys.specs -Wl,--gc-sections -T stm32f103.ld

C_SOURCES = main.c
ASM_SOURCES = startup_stm32f1xx.s

OBJECTS = $(C_SOURCES:.c=.o)
OBJECTS += $(ASM_SOURCES:.s=.o)

all: $(TARGET).elf $(TARGET).hex $(TARGET).bin size

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

%.hex: %.elf
	$(CP) -O ihex $< $@

%.bin: %.elf
	$(CP) -O binary -S $< $@

size: $(TARGET).elf
	@$(SZ) $<

clean:
	$(RM) $(OBJECTS) $(TARGET).elf $(TARGET).hex $(TARGET).bin $(TARGET).map

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

.PHONY: all clean flash size
