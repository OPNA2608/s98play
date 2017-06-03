%ifndef _MISC_FILEREAD_INC_
%define _MISC_FILEREAD_INC_

		; ファイル読み込み
		; cs:bx ファイル名
		; ds:dx 読み込みアドレス
FileRead:	push	0
		push	0

		push	ds			; ファイル オープン
		mov	ax, 0x3d00
		push	cs
		pop	ds
		push	dx
		mov	dx, bx
		int	0x21
		pop	dx
		pop	ds
		mov	bx, ax
		jc	short .failed
		push	ds
		push	bp
		mov	bp, sp
		jmp	short .read

.lp:		mov	ax, ds			; アドレス インクリメント
		add	ax, 0x800
		mov	ds, ax

.read:		mov	ah, 0x3f		; ファイル リード
		mov	cx, 0x8000
		int	0x21
		jc	short .close
		add	[bp + 4], ax
		adc	word [bp + 6], byte 0
		cmp	ax, cx
		je	short .lp
		clc
.close:		pushf				; ファイル クローズ
		mov	ah, 0x3e
		int	0x21
		popf
		pop	bp
		pop	ds
.failed:	pop	ax
		pop	dx
		ret

%endif	; _MISC_FILEREAD_INC_
