%ifndef _SOUND_S98_INC_
%define _SOUND_S98_INC_

%include 'misc/Cpu.mac'
%include 'sound/S98Header.inc'

; S98

	align	2

s98_dataaddr	dw	0, 0
s98_playaddr	dw	0, 0
s98_steptime	dw	0
S98Rate		dw	0
S98IntrCount	dw	0
S98LoopCount	db	0
S98EndOfData	db	0
S98TitlAddrL	dw	0
S98TitlAddrH	dw	0

%include 'sound/Port.inc'

		; PIT 割込
s98_intr:	push	ax
		push	ds

		MOVSEG	ds, cs

		inc	word [S98IntrCount]
		sub	word [s98_steptime], byte 1
		jc	short .step

		; EOI
.eoi:		call	PicSendEOI

		pop	ds
		pop	ax
		iret

		; 1 ステップ実行
.step:		cld
		pusha
		push	es

		les	si, [s98_playaddr]

.lp:		es lodsb
%ifdef S98HOOKCOMMAND
		call	S98HOOKCOMMAND
		jc	short .lp
%endif
		cmp	al, (SoundBoardMax * 2)
		jc	short .reg
		cmp	al, 0xfe
		jae	short .tick

		dec	si
		cmp	al, 0xfd
		jne	short .dataerror

.cmd_fd:	inc	byte [S98LoopCount]
		push	si
		les	si, [s98_dataaddr]
		mov	ax, [es:si + S98Header.LoopPoint + 0]
		mov	dx, [es:si + S98Header.LoopPoint + 2]
		ISZERO	ax, dx
		je	short .endofdata
		call	add_essi_32
		mov	[s98_playaddr + 2], es
		pop	ax
		jmp	short .lp

.endofdata:	pop	si
.dataerror:	inc	byte [S98EndOfData]
		jmp	short .storeoff

.tick:		mov	dx, 0
		jne	short .storetick
		call	s98_getdelta
.storetick:	mov	[s98_steptime], dx
		cmp	si, 0x8000
		jb	short .storeoff
		sub	si, 0x8000
		add	[s98_playaddr + 2], word 0x800
.storeoff:	mov	[s98_playaddr + 0], si
		pop	es
		popa
		jmp	.eoi

.reg:		and	ax, byte 15
		shl	ax, 2
		mov	bx, ax
		es lodsw
		call	SoundSendData
		jmp	short .lp

		; Delta値取得
s98_getdelta:	xor	cx, cx
.lp:		mov	ah, ch
		es lodsb
		test	al, 0x80
		je	short .ed
		and	al, 0x7f
		shl	ax, cl
		add	dx, ax
		add	cl, 7
		jmp	short .lp
.ed:		shl	ax, cl
		add	dx, ax
		inc	dx
		ret


		; 再生準備
S98Prepare:	cmp	byte [si + S98Header.Magic + 0], 'S'
		jne	short .err
		cmp	word [si + S98Header.Magic + 1], 0x3839		; '98'
		jne	short .err
		call	S98GetVersion
		jne	short .err
		mov	[cs:s98_dataaddr + 0], si
		mov	[cs:s98_dataaddr + 2], ds
		xor	bx, bx
		call	S98GetDevice
		clc
		ret
.err:		stc
		ret


		; デバイス情報
S98GetVersion:	mov	al, [si + S98Header.FormatVersion]
		sub	al, '0'
		cmp	al, 1
		je	short .exit
		cmp	al, 3
.exit:		ret


		; デバイス情報
S98GetDevice:	call	S98GetVersion
		cmp	al, 1
		je	short .default
		mov	ax, [si + S98Header.DeviceCount]
		cmp	bx, ax
		jae	short .default

		push	bx
		shl	bx, 4
		mov	ax, [si + bx + S98Header.DeviceType]
		pop	bx
		ret

.default:	cmp	bx, byte 1
		sbb	ax, ax
		and	ax, byte S98OPNA
		ret


; for use after s98_dataaddr is set up
S98GetVersion2:	push	si
		push	es
		les	si, [s98_dataaddr]

		call	S98GetVersion3

		pop	es
		pop	si
		ret


; for use with ES:SI set up
S98GetVersion3:	mov	al, [es:si + S98Header.FormatVersion]
		sub	al, '0'
		cmp	al, 1
		je	short .exit
		cmp	al, 3
.exit:		ret


; setup S98TitlAddr{L,H} with pointers to start of title data
; if result is 0000:0000, then either no title data or some kind of error
S98FindTitle:	push	ax
		push	bx
		push	dx
		push	si
		push	es
		les	si, [s98_dataaddr]

		mov	al, [es:si + S98Header.TagOffset + 0]
		mov	ah, [es:si + S98Header.TagOffset + 1]
		mov	bl, [es:si + S98Header.TagOffset + 2]
		mov	bh, [es:si + S98Header.TagOffset + 3]

		cmp	ax, 0
		jnz	short .someTitle

		cmp	bx, 0
		jz	short .exit

.someTitle:	push	ax
		call	S98GetVersion3
		jne	short .exit
		cmp	al, 1
		je	short .v1Title
		jmp	short .v3Title

.v1Title:	pop ax
		mov	[S98TitlAddrL], ax
		mov	[S98TitlAddrH], bx
		jmp	short .exit

.v3Title:	pop	ax
		; Skip "[S98]"
		add	ax, 5
		adc	bx, 0
		mov	[S98TitlAddrL], ax
		mov	[S98TitlAddrH], bx

		; if UTF-8 BOM is specified, skip over it
		call	S98GetTitleChar
		cmp	dl, 0xEF
		jne	short .v3UndoSkip2

		call	S98GetTitleChar
		cmp	dl, 0xBB
		jne	short .v3UndoSkip1

		call	S98GetTitleChar
		cmp	dl, 0xBF
		jne	short .v3Undo
		jmp	.exit

.v3UndoSub:	mov	ax, [S98TitlAddrL]
		mov     bx, [S98TitlAddrH]
		sub	ax, 1
		sbb	bx, 0
		mov	[S98TitlAddrL], ax
		mov	[S98TitlAddrH], bx
		ret

.v3Undo:	call    .v3UndoSub
.v3UndoSkip1:	call	.v3UndoSub
.v3UndoSkip2:	call	.v3UndoSub

.exit:		pop	es
		pop	si
		pop	dx
		pop	bx
		pop	ax
		ret


; get the next character of the title data
S98GetTitleChar:	push	si
		push	es
		push	bx
		les	si, [s98_dataaddr]

		push	es
		pop	ax

		; file data loaded in 0x8000 byte chunks across multiple segments
		; segment of data increased by 0x100 per 0x8000
		; high address = per 0x10000, so shift 1 extra
		mov	bx, [S98TitlAddrH]
		shl	bx, 8 + 4
		add	ax, bx

		; adjust segment according to 0x8000 chunk
		mov	bx, [S98TitlAddrL]
		and	bx, 0x8000
		shr	bx, 4
		add	ax, bx

		push	ax
		pop	es

		mov	bx, [S98TitlAddrL]
		; 0x8000 is max chunk size
		and	bx, 0x7fff
		add	si, bx

		mov	dl, [es:si]

		; increment title address
		mov	bx, [S98TitlAddrH]
		mov	ax, [S98TitlAddrL]
		inc	ax
		adc	bx, 0
		mov	[S98TitlAddrH], bx
		mov	[S98TitlAddrL], ax

.exit:		pop	bx
		pop	es
		pop	si
		ret


		; 再生中?
S98IsPlaying:	push	ax
		mov	ax, [s98_playaddr + 0]
		or	ax, [s98_playaddr + 2]
		pop	ax
		ret


		; 再生開始
S98Play:	push	si
		push	es

		call	S98IsPlaying
		jne	short .err

		les	si, [s98_dataaddr]
		mov	ax, [es:si + S98Header.DumpData + 0]
		mov	dx, [es:si + S98Header.DumpData + 2]
		ISZERO	ax, dx
		je	short .err
		call	add_essi_32
		mov	[s98_playaddr + 0], si
		mov	[s98_playaddr + 2], es
		and	word [s98_steptime], byte 0
		mov	byte [S98LoopCount], 0
		mov	byte [S98EndOfData], 0

		mov	dx, s98_intr		; インターバル タイマ設定
		call	PitInitialize
		call	PitGetClock
		les	si, [s98_dataaddr]
		call	s98_rate
		call	PitSetInterval
		call	S98GetRate
		mov	[S98Rate], ax
		clc
		jmp	short .exit

.err:		stc
.exit:		pop	es
		pop	si
		ret


		; 停止
S98Stop:	call	PitTerminate
		xor	ax, ax
		mov	[s98_playaddr + 0], ax
		mov	[s98_playaddr + 2], ax
		mov	[S98Rate], ax
		ret


		; S98の再生レートを算出
s98_rate:	push	bp
		push	dx
		push	ax
		call	s98_timerinfo
		call	Mul32_64
		push	dx
		push	ax
		mov	bp, sp
		call	s98_timerinfo2
		call	Div64_32
		pop	ax
		add	sp, byte 6
		pop	bx
		ret


		; S98の再生レート(Hz)を取得
S98GetRate:	call	s98_timerinfo
		mov	cx, ax
		call	s98_timerinfo2
		div	cx
		shr	cx, 1
		cmp	dx, cx
		jbe	short .skip
		inc	ax
.skip:		ret


		; タイマー情報1
s98_timerinfo:	mov	ax, [es:si + S98Header.TimerInfo + 0]
		mov	dx, [es:si + S98Header.TimerInfo + 2]
		ISZERO	ax, dx
		jne	short .exit
		add	ax, 10
.exit:		ret


		; タイマー情報2
s98_timerinfo2:	mov	ax, [es:si + S98Header.TimerInfo2 + 0]
		mov	dx, [es:si + S98Header.TimerInfo2 + 2]
		ISZERO	ax, dx
		jne	short .exit
		mov	ax, 1000
.exit:		ret


		; ES:SI += DX:AX
add_essi_32:	mov	cx, ax
		and	ax, byte 15
		add	si, ax
		shr	cx, 4
		shl	dx, 12
		mov	ax, es
		add	ax, dx
		add	ax, cx
		mov	es, ax
		ret


%include 'misc/Mul32_64.inc'
%include 'misc/Div64_32.inc'
%include 'io/Pic.inc'
%include 'io/Pit.inc'

%endif	; _SOUND_S98_INC_
