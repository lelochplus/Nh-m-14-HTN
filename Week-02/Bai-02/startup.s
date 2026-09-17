.syntax unified
.cpu cortex-m3
.thumb

.global g_pfnVectors
.global Reset_Handler

.word _estack

.section .text.Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    LDR r0, =_sidata
    LDR r1, =_sdata
    LDR r2, =_edata
    B LoopCopyDataInit

CopyDataInit:
    LDR r3, [r0], #4
    STR r3, [r1], #4

LoopCopyDataInit:
    CMP r1, r2
    BCC CopyDataInit

    LDR r1, =__bss_start__
    LDR r2, =__bss_end__
    B LoopFillZerobss

FillZerobss:
    MOVS r3, #0
    STR r3, [r1], #4

LoopFillZerobss:
    CMP r1, r2
    BCC FillZerobss

    BL SystemInit
    BL main
    BX lr

.size Reset_Handler, .-Reset_Handler

.section .isr_vector, "a", %progbits
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler

.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler

.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler

.weak SVC_Handler
.thumb_set SVC_Handler, Default_Handler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler

.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler

.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler

.weak Default_Handler
Default_Handler:
Infinite_Loop:
    B Infinite_Loop
