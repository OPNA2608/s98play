%ifndef _IO_SOFTWAIT_INC_
%define _IO_SOFTWAIT_INC_

		; 初期化
SoftWaitInit:	mov	cx, 100
		call	GetWaitCounter
		push	ax
		mov	cx, 3125 + 100
		call	GetWaitCounter
		pop	dx
		sub	ax, dx			; 3125ループのカウント

		cmp	ax, byte 1
		adc	ax, byte 0
		mov	[cs:SoftWaitCount + 1], ax
		ret


SoftWaitCount:	mov	cx, 0
		jcxz	.setup
.calc:		mov	dx, ax
		call	PitClockMode
		and	ax, 9 * 32 * 3
		add	ax, 9 * 32 * 13
		mul	dx
		div	cx
		ret
.setup:		push	ax
		call	SoftWaitInit
		mov	cx, ax
		pop	ax
		jmp	short .calc


		; ループのカウントを得る
GetWaitCounter:	pushf
		cli
		mov	ax, -1
		call	PitSetInterval
		loop	$
		call	PitGetCount
		popf
		not	ax
		ret


		; PIT カウンタ値
PitGetCount:	mov	al, 0x06
		out	0x77, al
		in	al, 0x71
		mov	ah, al
		in	al, 0x71
		xchg	al, ah
		ret

%include 'io/Pit.inc'

%endif	; _IO_SOFTWAIT_INC_
