; PC-9801-118関係

%ifndef _SOUND_YMF297_INC_
%define _SOUND_YMF297_INC_

%include 'sound/OpnIo.inc'

		; PC-9801-118 を見つける
Ymf297Find:	mov	dx, 0x148e
		mov	al, 0xff
		out	dx, al
		inc	dx
		in	al, dx
		cmp	al, 0xff
		jne	short .init
		ret

		; PC-9801-118 初期化
.init:		mov	byte [cs:Ymf297Reset], 0x90		; NOP
		call	Ymf297OPN3Mode

		; PC-9801-118 OPL3モードに設定
Ymf297OPL3Mode:	mov	dx, 0xa460
		mov	al, 0x03
		out	dx, al

		mov	dx, 0x0f4a
		mov	ax, 0x0180
		call	Ymf297WriteOPL3

		; New bit=1
		mov	dx, 0x148a
		mov	ax, 0x0505
		call	Ymf297WriteOPL3

		; CLR
		mov	ax, 0x0804
		call	Ymf297WriteOPL3
		mov	cx, 0x2000
.wait:		out	0x5f, al
		loop	.wait
		mov	al, 0x00
		call	Ymf297WriteOPL3.h

		; apply modes
		sub	dx, byte 2
		mov	ax, 0xf7f7
		call	Ymf297WriteOPL3

		stc
		ret

		; PC-9801-118 リセット
Ymf297Reset:	ret

		; PC-9801-118 OPN3モードに設定
Ymf297OPN3Mode:	mov	dx, 0xa460
		mov	al, 0x01
		out	dx, al

		mov	dx, 0x0f4a
		mov	ax, 0x0180
		call	Ymf297WriteOPL3

		mov	dx, 0x0188
		mov	ax, 0x2002
		call	OpnWriteData
		mov	al, 0x82
		call	OpnWriteData.h
		mov	cx, 0x2000
.wait:		out	0x5f, al
		loop	.wait
		mov	al, 0x02
		call	OpnWriteData.h
		mov	ax, 0xf7f7
		call	OpnWriteData

		mov	dx, 0x0f4a
		mov	ax, 0x0100
		call	Ymf297WriteOPL3
		ret

		; OPL3 レジスタ書き込み
Ymf297WriteOPL3:
		xchg	al, ah
		call	.write
		xchg	al, ah
.h:		inc	dx
		call	.write
		dec	dx
		ret
.write:		mov	cx, 10
.lp:		out	0x5f, al
		loop	.lp
		out	dx, al
		ret

%endif	; _SOUND_YMF297_INC_
