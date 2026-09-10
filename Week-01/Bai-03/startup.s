.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global Default_Handler

.section .isr_vector,"a",%progbits
.word 0x20005000
.word Reset_Handler

.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler
.word Default_Handler

.text

.thumb_func
Reset_Handler:
    bl main

1:
    b 1b

.thumb_func
Default_Handler:
    b Default_Handler
