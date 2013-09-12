#ifndef DESCRIPTOR_TABLES_H
#define DESCRIPTOR_TABLES_H

/*
 * This structure contains the value of one GDT entry. We use the attribute
 * 'packed' to tell the compiler not to change any of the alignment in the
 * structure.
 */
struct gdt_entry {
	uint16_t	limit_lo;    /* The lower 16 bits of the limit. */
	uint16_t	base_lo;     /* The lower 16 bits of the base. */
	uint8_t		base_mi;     /* The next 8 bits of the base. */
	uint8_t		access;      /* Access flags, determine what ring this segment can be used in. */
	uint8_t		granularity;
	uint8_t		base_hi;     /* The last 8 bits of the base. */
} __packed;

struct gdt_ptr {
	uint16_t	limit; /* The upper 16 bits of all selector limits. */
	uint32_t	base;  /* The address of the first gdt_entry_t struct. */
} __packed;


/* A struct describing an interrupt gate. */
struct idt_entry
{
	uint16_t	base_lo; /* The lower 16 bits of the address to jump to when this interrupt fires. */
	uint16_t	sel;     /* Kernel segment selector. */
	uint8_t		always0; /* This must always be zero. */
	uint8_t		flags;   /* More flags. See documentation. */
	uint16_t	base_hi; /* The upper 16 bits of the address to jump to. */
} __packed;

/* A struct describing a pointer to an array of interrupt handlers.
   This is in a format suitable for giving to 'lidt'. */
struct idt_ptr
{
	uint16_t	limit;
	uint32_t	base;  /* The address of the first element in our idt_entry_t array. */
} __packed;


void	init_descriptor_tables(void);


/* These extern directives let us access the addresses of our ASM ISR
   handlers. */
extern void  isr0(void);
extern void  isr1(void);
extern void  isr2(void);
extern void  isr3(void);
extern void  isr4(void);
extern void  isr5(void);
extern void  isr6(void);
extern void  isr7(void);
extern void  isr8(void);
extern void  isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

#endif /* ndef DESCRIPTOR_TABLES_H */
