global outb
global intb
global init_8259a

[bits 32]

outb:
	mov dx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	nop
	nop
	ret;

intb:
	mov dx, [esp + 4]
	mov ecx, [esp + 8]
	xor eax, eax
	in al, dx
	nop
	nop
	mov [ecx], eax
	ret

init_8259a:
	jmp @set
	_port  dw 0x20, 0x21, 0x21, 0x21, 0xa0, 0xa1, 0xa1, 0xa1
	_value db 0x11, 0x20, 0x04, 0x01, 0x11, 0x28, 0x02, 0x01
@set:
	pushad
	mov esi, _port
	mov edi, _value
	mov ecx, 8
.next:
	mov dx, [esi]
	mov al, [edi]
	out dx, al
	add esi, 2
	inc edi
	loop .next
.end:
	sti
	popad
	ret

