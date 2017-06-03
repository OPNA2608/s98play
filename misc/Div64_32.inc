%ifndef _MISC_DIV68_32_INC_
%define _MISC_DIV68_32_INC_

%include 'misc/Div32Sub.inc'
%include 'misc/Div48_32Sub.inc'

		; 64/32bit Š„‚èŽZ
Div64_32:	add	bp, byte 4
		push	bx
		push	ax
		push	dx
		call	Div32Sub
		mov	cx, ax
		mov	bx, dx
		pop	dx
		pop	ax
		dec	bp
		dec	bp
		push	cx
		push	ax
		push	dx
		push	bx
		call	Div48_32Sub
		pop	word [bp + 4]
		mov	bx, ax
		pop	dx
		pop	ax
		push	bx			; *
		dec	bp
		dec	bp
		call	Div48_32Sub
		pop	dx			; *
		pop	word [bp + 4]
		xchg	ax, [bp]
		xchg	dx, [bp + 2]
		pop	bx
		ret

%endif	; _MISC_DIV68_32_INC_
