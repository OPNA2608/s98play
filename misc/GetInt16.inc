%ifndef _MISC_GETINT16_INC_
%define _MISC_GETINT16_INC_

%include 'misc/SkipWhiteSpace.inc'

		; DS:SI Ç©ÇÁ 10êiêîÇìæÇÈ
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

%endif	; _MISC_GETINT16_INC_
