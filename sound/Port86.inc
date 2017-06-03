%ifndef _SOUND_PORT86_INC_
%define _SOUND_PORT86_INC_

; PC-9801-86 設定

	align	2

PORT86ADDR	equ	0xa460			; ポート

		; OPNA 有効
Port86Enable:	mov	dx, PORT86ADDR
		in	al, dx
		cmp	al, 0xff
		je	short .exit
		push	ax
		and	ax, 0x03fc
		or	al, ah
		out	dx, al
		pop	ax
.exit:		ret


		; OPNA ポート
Port86OpnaPort:	mov	dx, PORT86ADDR
		in	al, dx
		push	ax
		xor	dx, dx
		shr	al, 4
		cmp	al, 8
		ja	short .exit
		mov	dx, 0x0188
		sub	al, 3
		ror	al, 1
		cmp	al, 2
		adc	dh, 1
		stc
.exit:		pop	ax
		ret

%endif	; _SOUND_PORT86_INC_
