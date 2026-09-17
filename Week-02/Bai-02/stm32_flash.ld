MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vector))
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
  } > FLASH

  _sidata = LOADADDR(.data);

  .data : AT (_sidata)
  {
    _sdata = .;
    *(.data)
    *(.data*)
    _edata = .;
  } > RAM

  .bss :
  {
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > RAM
}
