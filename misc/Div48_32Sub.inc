%ifndef _MISC_DIV48_32SUB_INC_
%define _MISC_DIV48_32SUB_INC_

		; 48/32bit Š„‚èŽZ ([BP]/DA -> A:[BP])
Div48_32Sub:	mov	cx, 1
		xor	bx, bx
.lp:		shr	dx, 1
		rcr	ax, 1
		rcr	bx, 1
		cmp	dx, [bp + 4]
		jne	short .cmp48
		cmp	ax, [bp + 2]
		jne	short .cmp48
		cmp	bx, [bp]
		je	short .store
.cmp48:		jnc	short .next
.store:		sub	[bp], bx
		sbb	[bp + 2], ax
		sbb	[bp + 4], dx
		stc
.next:		adc	cx, cx
		jnc	short .lp
		mov	ax, cx
		ret

%endif	; _MISC_DIV48_32SUB_INC_
