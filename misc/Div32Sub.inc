%ifndef _MISC_DIV32SUB_INC_
%define _MISC_DIV32SUB_INC_

%include 'misc/Div48_32Sub.inc'

		; 32bit Š„‚èŽZ ([BP]/DA -> DA:[BP])
Div32Sub:	test	dx, dx
		jne	short .fast
		test	ax, ax
		je	short .zero
		mov	cx, ax
		xor	ax, ax
		xchg	ax, [bp + 2]
		div	cx
		push	ax
		mov	ax, [bp]
		div	cx
		mov	[bp], dx
		pop	dx
.zero:		ret
.fast:		xor	cx, cx
		xchg	cx, [bp + 4]
		push	cx
		call	Div48_32Sub
		pop	word [bp + 4]
		ret

%endif	;_MISC_DIV32SUB_INC_
