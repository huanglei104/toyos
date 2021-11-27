extern main

global inbyte
global outbyte
global systime
global do_switch
global kernel_setup

%define PDT			0x11000
%define STACK_SIZE	0x10000
%define STACK_LIMIT	0x500

[section .data]
	gdt	dq 0x1f
		dq 0xcf98000000ffff
		dq 0xcf92000000ffff
		dq 0x40960000000500
	a8259_port dw 0x20, 0x21, 0x21, 0x21, 0xa0, 0xa1, 0xa1, 0xa1, 0x21, 0xa1
 	 a8259_icw db 0x11, 0x20, 0x04, 0x01, 0x11, 0x28, 0x02, 0x01, 0x00, 0x00
 	mminfo times 128 db 0
	cmos_offset db 0x09, 0x08, 0x07, 0x04, 0x02, 0x00

[section .text]
	kernel_setup:
		; copy memory info
		cld
		mov esi, 0x7b00
		mov edi, mminfo
		mov ecx, 128 / 4
		rep movsd

		; init gdt
		mov esi, gdt
		mov edi, 0
		mov ecx, 8
		rep movsd
		lgdt [0]

		; init segment
		jmp 0x8: $ + 7
		mov ax, 0x10
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ax, 0x18
		mov ss, ax
		mov esp, STACK_LIMIT + STACK_SIZE - 1

		; enable page
		mov esi, PDT
		xor ebx, ebx
		mov ecx, 0x1000 / 4
	.clean:
		mov dword [esi + ebx], 0
		add ebx, 4
		loop .clean
	.init_pde:
		mov esi, PDT
		mov eax, PDT + 0x1000
		mov ecx, 0x20
	@next:
		mov ebx, eax
		and ebx, 0xfffff000
		or ebx, 0x7
		mov dword [esi], ebx
		add esi, 4
		add eax, 0x1000
		loop @next
	.init_pte:
		mov esi, PDT + 0x1000
		mov eax, 0
		mov ecx, 0x20 * 0x400
	@next1:
		mov ebx, eax
		and ebx, 0xfffff000
		or ebx, 0x7
		mov dword [esi], ebx
		add esi, 4
		add eax, 0x1000
		loop @next1
	.enable:
		mov eax, PDT
		mov cr3, eax
		mov eax, cr0
		or eax, 0x80000000
		mov cr0, eax

		; init i8259a
		mov esi, a8259_port
		mov edi, a8259_icw
		mov ecx, (a8259_icw - a8259_port) / 2
	@next2:
		mov dx, [esi]
		mov al, [edi]
		out dx, al
		add esi, 2
		inc edi
		loop @next2

		; init timer
		mov al, 0x34
		out 0x43, al	;使用二进制计数方式;工作方式2;读写方式先低后高;选择计数器0
		mov al, 0x9c
		out 0x40, al	;写入初始值低8位
		mov al, 0x2e
		out 0x40, al	;写入初始值高8位,将频率设置为100Hz

		; init rtc
		mov al, 0x0b
		or al, 0x80
		out 0x70, al	;选择寄存器B且阻断NMI
		mov al, 0x16
		out 0x71, al	;时间采用二进制格式和24小时制,只开放更新结束中断，关闭其他中断

		jmp main

	outbyte:
		push eax
		push edx
		mov edx, [esp + 12]
		mov eax, [esp + 16]
		out dx, al
		pop edx
		pop eax
		ret;

	inbyte:
		push eax
		push ebx
		push edx
		mov edx,[esp + 16]
		mov ebx, [esp + 20]
		in al, dx
		mov [ebx], al
		pop edx
		pop ebx
		pop eax
		ret

	systime:
		push eax
		push ebx
		push ecx
		push esi
		mov ebx, [esp + 20]

		mov al, 0xc
		out 0x70, al
		in al, 0x71		;读取C寄存器，使中断继续发生

		mov esi, cmos_offset
		mov ecx, 6
		xor eax, eax
	.next:
		mov al, [esi]
		out 0x70, al
		in al, 0x71
		mov [ebx], eax
		inc esi
		add ebx, 4
		loop .next

		mov al, 0x20
		out 0x20, al
		out 0xa0, al
		pop esi
		pop ecx
		pop ebx
		pop eax
		ret

do_switch:
		jmp far [esp]
		ret
