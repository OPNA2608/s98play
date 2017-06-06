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
		shl	ax, 4
		je	short .default

		push	bx
		add	bx, bx
		cmp	bx, ax
		sbb	ax, ax
		and	ax, [si + bx + S98Header.DeviceType]
		pop	bx
		ret

.default:	cmp	bx, byte 1
		sbb	ax, ax
		and	ax, byte S98OPNA
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
