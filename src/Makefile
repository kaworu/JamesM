# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler
# rule, as we use nasm instead of GNU as.

SOURCES=                \
	asm/boot.o          \
	common.o            \
	main.o              \
	freebsd/printf.o    \
	monitor.o           \
	asm/gdt_flush.o     \
	asm/idt_flush.o     \
	asm/interrupt.o     \
	isr.o               \
	descriptor_tables.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32 -I./include -I./freebsd/i386/include
LDFLAGS=-Tlink.ld -melf_i386
ASFLAGS=-felf

all: $(SOURCES) link

clean:
	-rm -f *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
