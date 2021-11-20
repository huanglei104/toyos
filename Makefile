CFLAGS = -Wall -m32 -masm=intel -fno-stack-protector  -mgeneral-regs-only -Os -fno-pic -fno-pie -ffreestanding
LDFLAGS = -m elf_i386 -T link.ld --gc-sections -s
INCS = -Iinclude
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, out/%.o, $(SRC))

kernel : bootloader out/boot.o $(OBJ)
	ld $(LDFLAGS) $(OBJ) out/boot.o -o out/kernel
	dd if=out/bootloader of=disk.img conv=notrunc
	dd if=out/kernel of=disk.img conv=notrunc seek=1

bootloader : asm/bootloader.asm
	nasm $< -o out/bootloader

out/boot.o : asm/boot.asm
	nasm  -f elf32 $< -o $@

out/%.o : %.c
	gcc $(CFLAGS) $(INCS) -c $< -o $@

lib.o : lib/*.c
	make -C lib/

clean:
	rm -rf out/*


