%ifndef _MISC_DIV48_32_INC_
%define _MISC_DIV48_32_INC_

%include 'misc/Div32Sub.inc'
%include 'misc/Div48_32Sub.inc'

		; 48/32bit Š„‚èŽZ
Div48_32:	inc	bp
		inc	bp
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
		push	bx
		call	Div48_32Sub
		pop	word [bp + 4]
		pop	dx
		xchg	ax, [bp]
		xchg	dx, [bp + 2]
		pop	bx
		ret

%endif	; _MISC_DIV48_32_INC_
