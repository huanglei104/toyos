global outbyte
global inbyte
global init_8259a

[bits 32]

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

init_8259a:
	jmp @set
	_port  dw 0x20, 0x21, 0x21, 0x21, 0xa0, 0xa1, 0xa1, 0xa1, 0x21
	_value db 0x11, 0x20, 0x04, 0x01, 0x11, 0x28, 0x02, 0x01, 0x0d
@set:
	pushad
	mov esi, _port
	mov edi, _value
	mov ecx, 9
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
