global outbyte
global inbyte
global init_8259a
global init_timer
global init_rtc
global read_time

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
	_port  dw 0x20, 0x21, 0x21, 0x21, 0xa0, 0xa1, 0xa1, 0xa1, 0x21, 0xa1
	_value db 0x11, 0x20, 0x04, 0x01, 0x11, 0x28, 0x02, 0x01, 0x00, 0x00
@set:
	pushad
	mov esi, _port
	mov edi, _value
	mov ecx, (_value - _port) / 2
.next:
	mov dx, [esi]
	mov al, [edi]
	out dx, al
	add esi, 2
	inc edi
	loop .next
.end:
	popad
	ret

init_timer:
	mov al, 0x34
	out 0x43, al	;使用二进制计数方式;工作方式2;读写方式先低后高;选择计数器0
	mov al, 0x9c
	out 0x40, al	;写入初始值低8位
	mov al, 0x2e
	out 0x40, al	;写入初始值高8位,将频率设置为100Hz
	ret

init_rtc:
	mov al, 0x0b
	or al, 0x80
	out 0x70, al	;选择寄存器B且阻断NMI
	mov al, 0x16
	out 0x71, al	;时间采用二进制格式和24小时制,只开放更新结束中断，关闭其他中断
	ret

read_time:
	jmp _set
	_cmos_offset db 0x09, 0x08, 0x07, 0x04, 0x02, 0x00
_set:
	push eax
	push ebx
	mov ebx, [esp + 12]

	mov al, 0xc
	out 0x70, al
	in al, 0x71		;读取C寄存器，使中断继续发生

	push ecx
	mov esi, _cmos_offset
	mov ecx, (_set - _cmos_offset) / 1
.next
	mov al, [esi]
	out 0x70, al
	in al, 0x71
	mov [ebx], al
	inc esi
	inc ebx
	loop .next
	pop ecx

	mov al, 0x20
	out 0x20, al
	out 0xa0, al
	pop ebx
	pop eax
	ret
