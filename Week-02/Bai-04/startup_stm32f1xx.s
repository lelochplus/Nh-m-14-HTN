.syntax unified
.cpu cortex-m3
.thumb

.global g_pfnVectors
.global Reset_Handler

.section .text.Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    ldr   r0, =_sdata
    ldr   r1, =_edata
    ldr   r2, =_sidata
1:
    cmp   r0, r1
    bge   2f
    ldr   r3, [r2], #4
    str   r3, [r0], #4
    b     1b
2:
    ldr   r0, =_sbss
    ldr   r1, =_ebss
    movs  r3, #0
3:
    cmp   r0, r1
    bge   4f
    str   r3, [r0], #4
    b     3b
4:
    bl    SystemInit
    bl    main
    bx    lr

.size Reset_Handler, .-Reset_Handler

.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
g_pfnVectors:
    .word   0x20005000 /* Stack pointer cơ bản */
    .word   Reset_Handler
    .word   Default_Handler

.section .text.Default_Handler,"ax",%progbits
Default_Handler:
    b .
