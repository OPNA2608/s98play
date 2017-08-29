%ifndef _MISC_FILEREADSUB_INC_
%define _MISC_FILEREADSUB_INC_

		; ファイル読み込み
		; bx ハンドル
		; ds:dx 読み込みアドレス
FileReadSub:	push	0
		push	0
		push	ds
		push	bp
		mov	bp, sp
		jmp	short .read
.lp:		mov	ax, ds			; アドレス インクリメント
		add	ah, 8
		mov	ds, ax
.read:		mov	ah, 0x3f		; ファイル リード
		mov	cx, 0x8000
		int	0x21
		jc	short .failed
		add	[bp + 4], ax
		adc	word [bp + 6], byte 0
		cmp	ax, cx
		je	short .lp
		clc
.failed:	pop	bp
		pop	ds
		pop	ax
		pop	dx
		ret

%endif	; _MISC_FILEREADSUB_INC_
