; PC-9801-118関係

		; PC-9801-118 を見つける
Find118:	mov	dx, 0x148e
		mov	al, 0xff
		out	dx, al
		inc	dx
		in	al, dx
		cmp	al, 0xff
		jne	short .init
		ret

		; PC-9801-118 初期化
.init:		mov	byte [cs:Reset118], 0x90		; NOP
		call	Set118OPN3Mode

		; PC-9801-118 OPL3モードに設定
Set118OPL3Mode:	mov	dx, 0xa460
		mov	al, 0x03
		call	WaitAndWriteReg

		mov	dx, 0x0f4a
		mov	ax, 0x0180
		call	Write118OPL3

		; New bit=1
		mov	dx, 0x148a
		mov	ax, 0x0505
		call	Write118OPL3

		; CLR
		mov	ax, 0x0804
		call	Write118OPL3
		mov	cx, 0x2000
.wait:		out	0x5f, al
		loop	.wait
		mov	al, 0x00
		call	Write118OPL3.h

		; apply modes
		sub	dx, byte 2
		mov	ax, 0xf7f7
		call	Write118OPL3

		stc
		ret

		; PC-9801-118 リセット
Reset118:	ret

		; PC-9801-118 OPN3モードに設定
Set118OPN3Mode:	mov	dx, 0xa460
		mov	al, 0x01
		call	WaitAndWriteReg

		mov	dx, 0x0f4a
		mov	ax, 0x0180
		call	Write118OPL3

		mov	dx, 0x0188
		mov	ax, 0x2002
		call	WriteOPNAData
		mov	al, 0x82
		call	WriteOPNAData.h
		mov	cx, 0x2000
.wait:		out	0x5f, al
		loop	.wait
		mov	al, 0x02
		call	WriteOPNAData.h
		mov	ax, 0xf7f7
		call	WriteOPNAData

		mov	dx, 0x0f4a
		mov	ax, 0x0100
		call	Write118OPL3
		ret

		; OPL3 レジスタ書き込み
Write118OPL3:	xchg	al, ah
		call	WaitAndWriteReg
		xchg	al, ah
.h:		inc	dx
		call	WaitAndWriteReg
		dec	dx
		ret

