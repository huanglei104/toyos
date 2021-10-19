global outbyte
global inbyte
global init_8259a
global init_timer
global init_rtc

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
	mov ecx, 10
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
	out 0x43, al
	mov al, 0x9c
	out 0x40, al
	mov al, 0x2e
	out 0x40, al
	ret

init_rtc:
	mov al, 0x0b                        ;RTC寄存器B
	or al, 0x80                         ;阻断NMI
	out 0x70, al
	;mov al, 0x12                        ;设置寄存器B，禁止周期性中断，开放更
	mov al, 0x72                        ;设置寄存器B，禁止周期性中断，开放更
	out 0x71, al                        ;新结束后中断，BCD码，24小时制
	mov al, 0x0c
	out 0x70, al
	in al, 0x71                         ;读RTC寄存器C，复位未决的中断状态
	in al, 0xa1                         ;读8259从片的IMR寄存器
	and al, 0xfe                        ;清除bit 0(此位连接RTC)
	out 0xa1,al                        ;写回此寄存器
	ret
