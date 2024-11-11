all: start

kernel.bin: entry.o kernel.o
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

entry.o: entry.asm
	nasm $< -f elf -o $@

kernel.o: kernel.c
	gcc -m32 -ffreestanding -c $< -o $@ -fno-pie

bootloader.bin: bootloader.asm
	nasm -f bin $^ -o $@

os-image.bin: bootloader.bin kernel.bin
	cat $^ > $@

start: os-image.bin
	qemu-system-x86_64 -fda $<