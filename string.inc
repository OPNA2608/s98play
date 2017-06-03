; 文字列操作系

		; ホワイト スペースをスキップする
SkipWhiteSpace:	lodsb
		dec	al
		cmp	al, ' '
		jc	short SkipWhiteSpace
		dec	si
		ret



		; DS:SI から 16進数を得る
GetHex16:	call	SkipWhiteSpace
		xor	dx, dx
		mov	ah, dh
.lp:		lodsb
		sub	al, '0'
		cmp	al, 10
		jc	short .adds
		sub	al, 0x10
		jbe	short .break
		and	al, ~0x20
		add	al, 9
		cmp	al, 16
		jnc	short .break
.adds:		shl	dx, 4
		or	dl, al
		inc	ah
		cmp	ah, 4
		jc	short .lp
		inc	si
.break:		dec	si
		cmp	ah, 1
		mov	ax, dx
		ret



		; 10進数を得る
GetInt16:	call	SkipWhiteSpace
		xor	dx, dx
		mov	ah, dh
.lp:		lodsb
		sub	al, '0'
		cmp	al, 10
		jnc	short .break
		imul	dx, dx, 10
		add	dl, al
		adc	dh, 0
		inc	ah
		cmp	ah, 5
		jc	short .lp
		inc	si
.break:		dec	si
		cmp	ah, 1
		mov	ax, dx
		ret



		; ES:DI に 16進数文字列をストアする
StoreHex16:	mov	cx, 4
.lp:		rol	ax, 4
		push	ax
		and	al, 15
		add	al, 48
		cmp	al, 58
		jc	short .store
		add	al, 7
.store:		stosb
		pop	ax
		loop	.lp
		ret
