#ifndef ISR_H
#define ISR_H
/*
 * isr.h -- Interface and structures for high level interrupt service routines.
 * Part of this code is modified from Bran's kernel development tutorials.
 * Rewritten for JamesM's kernel development tutorials.
 */
#include <common.h>

#define IRQ0	32
#define IRQ1	33
#define IRQ2	34
#define IRQ3	35
#define IRQ4	36
#define IRQ5	37
#define IRQ6	38
#define IRQ7	39
#define IRQ8	40
#define IRQ9	41
#define IRQ10	42
#define IRQ11	43
#define IRQ12	44
#define IRQ13	45
#define IRQ14	46
#define IRQ15	47

struct cpu_registers
{
   uint32_t	ds;                                     /* Data segment selector */
   uint32_t	edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha. */
   uint32_t	int_no, err_code;                       /* Interrupt number and error code (if applicable) */
   uint32_t	eip, cs, eflags, useresp, ss;           /* Pushed by the processor automatically. */
};

/*
 * Enables registration of callbacks for interrupts or IRQs.  For IRQs, to ease
 * confusion, use the #defines above as the first parameter.
 */
typedef	void (*isrhdl_t)(struct cpu_registers);

void register_interrupt_handler(uint32_t n, isrhdl_t handler);
#endif /* ndef ISR_H */
