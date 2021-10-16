CFLAGS = -Wall -m32 -masm=intel -fno-builtin -fno-builtin-function -fno-stack-protector -nolibc  -nostdlib -mgeneral-regs-only -Os -fno-pic -fno-pie
LDFLAGS = -m elf_i386 -T link.ld --gc-sections -s
INCS = -I./include
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

build : loader kernel
	dd if=bootloader of=disk.img conv=notrunc
	dd if=kernel of=disk.img conv=notrunc seek=1

clean:
	rm -rf bootloader kernel $(OBJ) core.o

loader : bootloader.asm
	nasm bootloader.asm -o bootloader

kernel : $(OBJ) core.o
	ld $(LDFLAGS) -o kernel $(OBJ) core.o

%.o : %.c
	gcc $(CFLAGS) $(INCS) -c $< -o $@

core.o : core.asm
	nasm core.asm -f elf32 -o core.o
