%ifndef _MISC_GETHEX16_INC_
%define _MISC_GETHEX16_INC_

%include 'misc/SkipWhiteSpace.inc'

		; DS:SI Ç©ÇÁ 16êiêîÇìæÇÈ
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

%endif	; _MISC_GETHEX16_INC_
