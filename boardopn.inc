; OPN 関係

		; OPNA を見つける
FindOPNA:	mov	dx, 0xa460
		mov	al, 1
		out	dx, al

		mov	dx, 0x0088
		mov	ah, 0xff
.lp1:		call	ReadOPNAData
		cmp	al, 0xff
		jc	short .found
		inc	dh
		cmp	dh, 4
		jc	short .lp1
		ret

.found:		mov	ax, 0x2983
		call	WriteOPNAData
		mov	ax, dx
		stc
		ret

		; OPNA データを読み取る
ReadOPNAData:	mov	al, ah
		call	WaitAndWriteReg
		add	dx, byte 2
		call	WaitAndReadReg
		sub	dx, byte 2
		ret

		; OPNA データを書き込む
WriteOPNAData:	xchg	al, ah
		call	WaitAndWriteReg
		xchg	al, ah
.h:		add	dx, byte 2
		call	WaitAndWriteReg
		sub	dx, byte 2
		ret

		; レジスタ リード
WaitAndReadReg:	mov	cx, 10
.lp:		out	0x5f, al
		loop	.lp
		in	al, dx
		ret

		; レジスタ ライト
WaitAndWriteReg:mov	cx, 10
.lp:		out	0x5f, al
		loop	.lp
		out	dx, al
		ret
