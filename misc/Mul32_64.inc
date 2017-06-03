%ifndef _MISC_MUL32_64_INC_
%define _MISC_MUL32_64_INC_

		; 32bit Š|‚¯ŽZ -> 64bit
Mul32_64:	push	bp
		mov	bp, sp
		push	dx
		push	ax		; A,D,BP,IP,C,B

		mov	ax, [bp + 6]	; B
		mul	dx		; D

		xchg	ax, [bp + 4]	; C<->R2
		xchg	dx, [bp + 6]	; B<->R3

		push	dx
		push	ax		; C,B,A,D,BP,IP,R2,R3

		mul	word [bp - 4]	; A
		push	dx
		push	ax		; R0,R1,C,B,A,D,BP,IP,R2,R3

		mov	ax, [bp - 8]	; C
		mul	word [bp - 2]	; D
		add	[bp - 10], ax
		adc	[bp + 4], dx
		adc	word [bp + 6], byte 0

		mov	ax, [bp - 6]	; B
		mul	word [bp - 4]	; A
		add	[bp - 10], ax
		adc	[bp + 4], dx
		adc	word [bp + 6], byte 0

		pop	ax
		pop	dx
		mov	sp, bp
		pop	bp
		ret

%endif	;_MISC_MUL32_64_INC_
