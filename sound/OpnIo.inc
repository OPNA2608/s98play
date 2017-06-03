%ifndef _SOUND_OPNIO_INC_
%define _SOUND_OPNIO_INC_

; OPN I/O 関係

		; OPN ウェイト
OpnWait:	mov	cx, 0x20
		push	ax
.lp:		out	0x5f, al
		in	al, dx
		test	al, al
		jns	short .exit
		loop	.lp
.exit:		pop	ax
		ret

		; OPN データを読み取る
OpnReadData:	mov	al, ah
		call	OpnWaitAndWriteReg
.h:		inc	dx
		inc	dx
		call	OpnWaitAndReadReg
		dec	dx
		dec	dx
		ret

		; OPN データを書き込む
OpnWriteData:	xchg	al, ah
		call	OpnWaitAndWriteReg
		xchg	al, ah
.h:		inc	dx
		inc	dx
		call	OpnWaitAndWriteReg
		dec	dx
		dec	dx
		ret

		; レジスタ リード
OpnWaitAndReadReg:
		call	OpnWait
		in	al, dx
		ret

		; レジスタ ライト
OpnWaitAndWriteReg:
		call	OpnWait
		out	dx, al
		ret

		; 割り込み番号を取得
OpnGetIrq:	call	OpnWait
		mov	al, 0xff
		jcxz	.err
		mov	ah, 0x07
		call	OpnReadData
		and	al, 0x3f
		or	al, 0x80
		call	OpnWriteData.h
		mov	ah, 0x0e
		call	OpnReadData
		shr	al, 6
		push	bx
		mov	bx, .irq_table
		xlat
		pop	bx
.err:		ret
.irq_table	db	0x03, 0x0d, 0x0a, 0x0c

%endif	; _SOUND_OPNIO_INC_
