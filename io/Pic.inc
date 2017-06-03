%ifndef _IO_PIC_INC_
%define _IO_PIC_INC_

; ---- PIC

; 割り込みマスクをストアする
PicStore:	in	al, 0x0a
		mov	ah, al
		in	al, 0x02
		ret


; 割り込みマスクをリストアする
PicRestore:	out	0x02, al
		mov	al, ah
		out	0x0a, al
		ret


; 割り込みをマスクする DL:IRQ
PicEnable:	mov	cl, dl
		mov	ch, 1
		and	cl, 7
		shl	ch, cl
		mov	cl, dl
		and	dx, byte 8
		add	dx, byte 2
		in	al, dx
		mov	ah, al
		or	al, ch
		test	cl, 0x80
		jne	short .output
		xor	al, ch
.output:	out	dx, al
		cmp	al, ah
		ret


; EOI を送る
PicSendEOI:	mov	al, 0x0b
		out	0x00, al
		out	0x5f, al
		in	al, 0x00
		or	al, al
		jns	short .master
		mov	al, 0x20
		out	0x08, al
		out	0x5f, al
		mov	al, 0x0b
		out	0x08, al
		out	0x5f, al
		in	al, 0x08
		or	al, al
		jne	short .ed
.master:	mov	al, 0x20
		out	0x00, al
.ed:		ret


; ---- Vector

		; ベクタを設定する
VectStore:	mov	ah, 0x35
		push	bx
		push	es
		push	ax
		int	0x21
		pop	ax
		mov	ah, 0x25
		int	0x21
		mov	ax, bx
		mov	dx, es
		pop	es
		pop	bx
		ret

		; ベクタを復帰する
VectRestore:	push	ax
		mov	ax, ds
		or	ax, dx
		pop	ax
		je	short .err
		mov	ah, 0x25
		int	0x21
		clc
		ret
.err:		stc
		ret

%endif	; _IO_PIC_INC_
