%ifndef _MISC_FILEGETSIZE_INC_
%define _MISC_FILEGETSIZE_INC_

		; ファイル サイズを取得
		; BX: ハンドル -> DX:AX サイズ
FileGetSize:	push	bp
		mov	bp, sp
		mov	ax, 0x4201
		xor	cx, cx
		xor	dx, dx
		int	0x21
		jc	short .exit
		push	ax
		push	dx
		mov	ax, 0x4202
		xor	cx, cx
		xor	dx, dx
		int	0x21
		jc	short .exit
		xchg	ax, [bp - 2]
		xchg	dx, [bp - 4]
		mov	cx, dx
		mov	dx, ax
		mov	ax, 0x4200
		int	0x21
		pop	dx
		pop	ax
.exit:		mov	sp, bp
		pop	bp
		ret


%endif	; _MISC_FILEGETSIZE_INC_
