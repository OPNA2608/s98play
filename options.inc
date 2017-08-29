
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
		cmp	al, 'S'
		je	short OptionSoftWait
		cmp	al, 'W'
		je	short OptionWait
		stc
.catch:		jnc	short .lp
		ret

		; ソフトウェイト
OptionSoftWait:	or	byte [options], OPT_FLAG_SWAIT
		ret

		; ポート番号を得る
OptionPort:	or	byte [options], OPT_DEF_PORT
		mov	di, SoundPorts
.lp:		call	GetHex16
		jc	short .skipaddr
		mov	[di + SoundPort.Addr], ax
.skipaddr:	call	optioniscomma
		jne	short .error
		call	GetHex16
		jc	short .skipdata
		mov	[di + SoundPort.Data], ax
.skipdata:	call	optioniscomma
		jne	short .exit
		add	di, byte SoundPort_size
		cmp	di, SoundPorts + (SoundBoardMax * 2 * SoundPort_size)
		jc	short .lp
.error:		stc
.exit:		ret

		; ウェイト値を得る
OptionWait:	or	byte [options], OPT_DEF_WAIT
		call	GetInt16
		jc	short .addr
		mov	[SoundDataWait], ax
		mov	dx, 65536 * 17 / 83
		mul	dx
		mov	[SoundAddrWait], dx
.addr:		call	optioniscomma
		jne	short .exit
		call	GetInt16
		jc	short .exit
		mov	[SoundAddrWait], ax
.exit:		ret

		; カンマかどうか
optioniscomma:	lodsb
		cmp	al, ','
		je	short .exit
		cmp	al, ' ' + 1
		cmc
		dec	si
.exit:		ret

%include 'misc/GetHex16.inc'
%include 'misc/GetInt16.inc'
