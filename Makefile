CFLAGS = -m32 -Os -fno-builtin -fno-builtin-function -fno-stack-protector -nolibc  -nostdlib
LDFLAGS = -m elf_i386 -T link.ld --gc-sections -s
INCS = -I./include
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

build : loader kernel
	dd if=bootloader of=disk.img conv=notrunc
	dd if=kernel of=disk.img conv=notrunc seek=1

loader : bootloader.asm
	nasm bootloader.asm -o bootloader

kernel : $(OBJ)
	ld $(LDFLAGS) -o kernel $(OBJ)

%.o : %.c
	gcc $(CFLAGS) $(INCS) -c $< -o $@

