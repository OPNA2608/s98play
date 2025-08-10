	USE16
	CPU	186
	ORG	0x100
	jmp	Main

MsgOpnaYes	db	"YM2608 found", 0x0d, 0x0a, '$'
MsgOpnaNo	db	"YM2608 missing", 0x0d, 0x0a, '$'

MsgTryingPort	db	"Trying possible port: 0x"
MsgTryingVal	db	"0000", 0x0d, 0x0a, '$'

MsgResponsePort	db	"Received response: 0x"
MsgResponseVal	db	"00", 0x0d, 0x0a, '$'

; misc/NibbleToAscii.inc
NibbleToAscii:	and	al, 15
		cmp	al, 10
		jc	short .skip
		add	al, 7
.skip:		add	al, '0'
		ret

; custom
StoreHex8:	push	cx
		push	ax
		mov	cx, 2
.lp:		rol	ax, 4
		push	ax
		call	NibbleToAscii
		stosb
		pop	ax
		loop	.lp
		pop	ax
		pop	cx
		ret

; misc/StoreHex16.inc
StoreHex16:	push	cx
		push	ax
		mov	cx, 4
.lp:		rol	ax, 4
		push	ax
		call	NibbleToAscii
		stosb
		pop	ax
		loop	.lp
		pop	ax
		pop	cx
		ret

; sound/Port86.inc
; if invalid response, not a YM2608(?)
; TODO: is this an invalid write on YM2203?
Port86Enable:	mov	dx, 0xa460
		in	al, dx
		cmp	al, 0xff
		je	short .exit

		; ?
		push	ax
		and	ax, 0x03fc
		or	al, ah
		out	dx, al
		pop	ax
.exit:		ret


; sound/OpnIo.inc
OpnWait:	push	ax
		mov	ah, 0x20

.lp:		out	0x5f, al
		in	al, dx
		test	al, al
		jns	short .exit
		dec	ah
		jns	short .lp

.exit:		pop	ax
		ret

; sound/OpnIo.inc
OpnWaitAndReadReg:
		call	OpnWait
		in	al, dx
		ret

; sound/OpnIo.inc
OpnWaitAndWriteReg:
		call	OpnWait
		out	dx, al
		ret

; sound/OpnIo.inc
OpnReadData:	mov	al, ah
		call	OpnWaitAndWriteReg
		add	dl, 2
		call	OpnWaitAndReadReg
		sub	dl, 2
		ret

; sound/OpnIo.inc
OpnWriteData:	xchg	al, ah
		call	OpnWaitAndWriteReg
		xchg	al, ah
		add	dl, 2
		call	OpnWaitAndWriteReg
		sub	dl, 2
		ret


; sound/Opna.inc
; read register 0xFF from sound card, check response
OpnaIsReady:	push	dx

		mov	ax, dx
		mov	di, MsgTryingVal
		call	StoreHex16
		mov	ah, 9
		mov	dx, MsgTryingPort
		int	0x21

		; TODO is this an invalid read on YM2203 (open bus) or just a not-well-defined response?
		pop	dx
		mov	ah, 0xff
		call	OpnReadData

		push	dx
		push	ax
		mov	ah, al
		mov	di, MsgResponseVal
		call	StoreHex8
		mov	ah, 9
		mov	dx, MsgResponsePort
		int	0x21

		pop	ax
		pop	dx
		cmp	al, 0xff
		ret

; sound/Opna.inc
; check for YM2608
OpnaFind:	push	cx
		xor	cx, cx
		mov	ah, 1
		call	Port86Enable
		; missing check for success?

		; try 0x0088
.try0088:	mov	dx, 0x0088
		call	OpnaIsReady
		jnc	short .try0188
		mov	cx, dx

		; try 0x0188
.try0188:	inc	dh
		call	OpnaIsReady
		jnc	short .try0288
		mov	cx, dx

		; try 0x0288
.try0288:	inc	dh
		call	OpnaIsReady
		jnc	short .try0388
		mov	cx, dx

		; try 0x0388
.try0388:	inc	dh
		call	OpnaIsReady
		jnc	short .checkFound
		mov	cx, dx

.checkFound:	cmp	cx, 0x0000
		je	.exit

.found:		mov	ax, 0x2983
		mov	dx, cx
		call	OpnWriteData
		mov	ax, dx
		stc
.exit:		pop	cx
		ret


Main:		call	OpnaFind
		jc	short .foundOpna
		jmp	short .noOpna

.foundOpna:	mov	ah, 9
		mov	dx, MsgOpnaYes
		int	0x21
		jmp	short .exit

.noOpna:	mov	ah, 9
		mov	dx, MsgOpnaNo
		int	0x21
		jmp	short .exit

.exit:		mov	ax, 0x4c00
		int	0x21
