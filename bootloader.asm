	KERNEL_LOAD_ADDR equ 0x100000

	org 0x7c00

;detect memory
	mov word [0x7b00], 0
	mov ax, 0
	mov es, ax
	mov di, 0x7b04
	mov ebx, 0
	mov ecx, 20
	mov edx, 0x534d4150
.next_ards:
	mov eax, 0xe820
	int 0x15
	add word [0x7b00], 1
	add di, 20
	cmp ebx, 0
	jne .next_ards

;load gdt
	lgdt [gdt_start]

;jump to protected mode
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp 0x8:.protected_mode

	[bits 32]
.protected_mode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	;栈空间从0x7e00 到 0x8e00
	mov ax, 0x18
	mov ss, ax
	mov esp, 0x8e00
	;读取255个扇区，所以kernel的大小不能超过255 * 512 byte
	mov al, 255
	mov ebx, 1
	mov esi, KERNEL_LOAD_ADDR
	call read_disk

;重定位内核
	xor eax, eax
	xor ecx, ecx
	mov ebp, KERNEL_LOAD_ADDR
	add ebp, [KERNEL_LOAD_ADDR + 28]
	mov ax, [KERNEL_LOAD_ADDR + 42]
	mov cx, [KERNEL_LOAD_ADDR + 44]
.load_elf_segment:
	push ecx;
	mov esi, [ds:ebp + 4]
	add esi, KERNEL_LOAD_ADDR;
	mov edi, [ds:ebp + 8]
	mov ecx, [ds:ebp + 16]
	cld
	rep movsb
	pop ecx
	add ebp, eax
	loop .load_elf_segment

;跳转到kernel_entry执行
	jmp [KERNEL_LOAD_ADDR + 24];

	;al 扇区数, ebx 低28位起始扇区号 esi 数据的地址
read_disk:
	push eax
	push ecx
	push edx
	push esi

	mov cl, al
	mov dx, 0x1f2
	out dx, al
	inc dx
	mov eax, ebx
	out dx, al
	inc dx
	shr eax, 8
	out dx, al
	shr eax, 8
	inc dx
	out dx, al
	shr eax, 8
	and al, 0xef
	or al, 0xe0
	inc dx
	out dx, al
	inc dx
	mov al, 0x20
	out dx, al

.wait_disk:
	in al, dx
	and al, 0x88
	cmp al, 0x8
	jnz .wait_disk
	and ecx, 0xff
	shl ecx, 8
	mov dx, 0x1f0

.copy_data:
	in ax, dx
	mov [esi], ax
	add esi, 2
	loop .copy_data

	pop esi
	pop edx
	pop ecx
	pop eax
	ret

	[bits 16]
gdt_start:
	dw (gdt_end - gdt_start)
	dd gdt_start
	dw 0
	dq 0xcf98000000ffff
	dq 0xcf92000000ffff
	dq 0x40960000007e00
gdt_end:

	times 510 - ($ - $$) db 0
	dw 0xaa55
