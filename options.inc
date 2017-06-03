
		; オプション解析
ParseOption:	mov	si, 0x80
		lodsb
		and	ax, byte 0x7f
		mov	bx, ax
		mov	byte [bx + si], bh
		xor	bx, bx

.lp:		call	SkipWhiteSpace
		lodsb
		test	al, al
		je	short .end
		cmp	al, '/'
		je	short .opt
		cmp	al, '-'
		je	short .opt

		; ファイル名を得る
		lea	bx, [si - 1]
.s98nameloop:	lodsb
		cmp	al, ' '
		ja	short .s98nameloop
		mov	byte [si - 1], 0
		test	al, al
		jne	short .lp
.end:		clc
		ret

.opt:		push	.catch
		lodsb
		and	al, ~0x20
		cmp	al, 'P'
		je	short OptionPort
		cmp	al, 'W'
		je	short OptionWait
		stc
.catch:		jnc	short .lp
		ret

		; ポート番号を得る
OptionPort:	mov	di, ioregisters
.lp:		push	cx
		call	GetHex16
		pop	cx
		jc	short .skip
		cmp	di, ioregisters + (12 * 2)
		jae	short .skip
		mov	[di], ax
		or	byte [options], OPT_DEF_PORT
.skip:		add	di, byte 2
		lodsb
		cmp	al, ','
		je	short .lp
		dec	si
		cmp	al, ' ' + 1
		cmc
		ret

		; ウェイト値を得る
OptionWait:	call	GetInt16
		jc	short .err
		cmp	ax, byte 1
		adc	ax, byte 0
		mov	[wait_count], ax
		or	byte [options], OPT_DEF_WAIT
.err:		ret
