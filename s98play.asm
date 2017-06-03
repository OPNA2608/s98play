
	USE16
	CPU	186

OPT_DEF_PORT	equ	0x01
OPT_DEF_WAIT	equ	0x02
OPT_FLAG_OPL	equ	0x04

	ORG	0x100

		jmp	Main

options		db	0
dataoff		dw	0
dataseg		dw	0

playoff		dw	0
playseg		dw	0

steptime	dw	0

vect08off	dw	0
vect08seg	dw	0

cycles_per_sec	dw	0
wait_count	dw	8

ioregisters	dw	0, 0, 0, 0
		dw	0, 0, 0, 0
		dw	0, 0, 0, 0

		struc	S98Header
.Magic		resb	3
.FormatVersion	resb	1
.TimerInfo	resw	2
.TimerInfo2	resw	2
.Compressing	resd	1
.TagOffset	resd	1
.DumpData	resw	2
.LoopPoint	resw	2
.DeviceCount	resd	1
.DeviceType	resd	1
.DeviceClock	resd	1
.Pan		resd	1
		resd	1
		endstruc



; ---- PIT

		; PIT 割込設定
PitInit:	cli
		; ベクタ アドレス退避
		mov	ax, 0x3508
		int	0x21
		mov	[vect08off], bx
		mov	[vect08seg], es
		; ベクタ アドレス設定
		mov	ax, 0x2508
		mov	dx, Int08
		int	0x21
		; インターバル タイマ値設定
		mov	al, 0x36
		out	0x77, al
		out	0x5f, al

		push	ds
		xor	ax, ax
		mov	ds, ax
		test	byte [ds:0x0501], 0x80
		pop	ds

		mov	dx, 2457600 >> 16
		mov	ax, 2457600 & 0xffff
		je	short .calc
		mov	dx, 1996800 >> 16
		mov	ax, 1996800 & 0xffff
.calc:		div	word [cycles_per_sec]
		out	0x71, al
		out	0x5f, al
		mov	al, ah
		out	0x71, al
		; 割込マスク解除
		in	al, 0x02
		out	0x5f, al
		and	al, 0xfe
		out	0x02, al
		sti
		ret

PitDeinit:	; 割込解放
		cli
		; ベクタ復帰
		push	ds
		lds	dx, [vect08off]
		mov	ax, 0x2508
		int	0x21
		pop	ds
		; 割込マスク設定
		in	al, 0x02
		or	al, 1
		out	0x5f, al
		out	0x02, al
		sti
		ret

		; PIT 割込
Int08:		push	ax
		push	cx
		push	dx

		sub	word [cs:steptime], byte 1
		jc	short .step

		; EOI
.eoi:		mov	al, 0x20
		out	0x00, al
		pop	dx
		pop	cx
		pop	ax
		iret

		; 1 ステップ実行
.step:		cld
		push	bx
		push	si
		push	ds
		lds	si, [cs:playoff]

.lp:		xor	dx, dx
		lodsb
		cmp	al, 6
		jc	short .reg
		inc	al
		je	short .cmd_ff
		inc	al
		je	short .cmd_fe

		dec	si
		mov	[cs:playoff], si

		inc	al
		jne	short .exit

.cmd_fd:	lds	si, [cs:dataoff]
		mov	ax, [si + S98Header.LoopPoint + 0]
		mov	dx, [si + S98Header.LoopPoint + 2]
		call	Add_dsi_32
		jc	short .exit
		mov	[cs:playseg], ds
		jmp	short .lp

.cmd_fe:	call	GetDelta
.cmd_ff:	mov	[cs:steptime], dx
		cmp	si, 0x8000
		jb	short .storeoff
		sub	si, 0x8000
		add	[cs:playseg], word 0x800
.storeoff:	mov	[cs:playoff], si
.exit:		pop	ds
		pop	si
		pop	bx
		jmp	.eoi

.reg:		and	ax, byte 15
		shl	ax, 2
		mov	bx, ax
		lodsw
		call	SendSoundData
		jmp	short .lp

		; Delta値取得
GetDelta:	xor	cx, cx
		xor	dx, dx
.lp:		mov	ah, ch
		lodsb
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

		; OPN/OPL のアドレス/データ セット
SendSoundData:	mov	dx, [cs:ioregisters + bx + 0]
		cmp	dx, byte 0
		je	short .skip
		out	dx, al
		mov	cx, [cs:wait_count]
.w1:		out	0x5f, al
		loop	.w1
		mov	dx, [cs:ioregisters + bx + 2]
		xchg	al, ah
		out	dx, al
		xchg	al, ah
		mov	cx, [cs:wait_count]
.w2:		out	0x5f, al
		loop	.w2
.skip:		ret

		; OPN/OPL をクリア
ClearBoards:	xor	bx, bx
		test	byte [options], OPT_FLAG_OPL
		jne	short ClearOPL

ClearOPNA:	mov	ax, 0xff07
		call	SendSoundData
		mov	ax, 0xff28
.lp:		inc	ah
		cmp	ah, 3
		je	short .lp
		call	SendSoundData
		cmp	ah, 7
		jb	short .lp
		ret

ClearOPL:	xor	bx, bx
		mov	ax, 0x00bd
		call	SendSoundData
		call	.reset
		add	bx, byte 4
		call	.reset
		mov	ax, 0x0004
		jmp	SendSoundData

.reset:		mov	ax, 0x00b0
.lp1:		call	SendSoundData
		inc	al
		cmp	al, 0xb9
		jc	short .lp1

		mov	ax, 0x00e0
.lp2:		call	SendSoundData
		inc	al
		mov	dl, al
		and	dl, 7
		cmp	dl, 6
		jc	short .lp2
		add	al, 2
		cmp	al, 0xf6
		jc	short .lp2
		ret



; ---- メイン

Main:		cld
		; メモリ アドレス設定
		mov	ax, EndOfProgram + 512
		add	ax, byte 15
		and	ax, byte 0xfff0
		mov	sp, ax
		mov	[dataoff], sp
		mov	[dataseg], cs

		call	ParseOption
		mov	dx, MsgInvalidOpt
		jc	short .err
		test	bx, bx
		mov	dx, MsgNonOpt
		je	short .err

		; ファイル読み込み
		push	ds
		lds	dx, [dataoff]
		call	ReadFile
		pop	ds
		mov	dx, MsgReaderr
		jc	short .err

		; S98 チェック
		push	ds
		lds	dx, [dataoff]
		call	S98Check
		pop	ds
		mov	dx, MsgInvalidS98
		jnc	short SetPorts

.err:		mov	ah, 9
		int	0x21
		mov	ax, 0x4c00
		int	0x21

SetPorts:	test	byte [options], OPT_DEF_PORT
		jne	short Playing
		call	CheckBoards

Playing:	and	word [steptime], byte 0
		call	PitInit

.lp:		hlt
		hlt
		hlt
		hlt
		mov	ah, 0x06
		mov	dl, 0xff
		int	0x21
		je	short .lp

		call	PitDeinit
		call	ClearBoards
		call	Reset118

		mov	ax, 0x4c00
		int	0x21

		; ボード チェック
CheckBoards:	test	byte [options], OPT_FLAG_OPL
		jne	short CheckOplBoards

		; OPN ボード チェック
CheckOpnBoards:	call	FindOPNA
		jc	short .found
		ret
.found:		mov	[ioregisters + 0], ax
		add	al, 2
		mov	[ioregisters + 2], ax
		add	al, 2
		mov	[ioregisters + 4], ax
		add	al, 2
		mov	[ioregisters + 6], ax
		sub	al, 6
		mov	dx, MsgOPNA

PrintBoardFound:push	ax
		mov	ah, 9
		int	0x21
		pop	ax
		mov	di, MsgFoundPort
		call	StoreHex16
		mov	ah, 9
		mov	dx, MsgFoundAt
		int	0x21
		stc
		ret


		; OPL ボード チェック
CheckOplBoards:	call	FindSB16
		jc	short .sb16
		call	Find118
		jc	short .board118
		ret

.sb16:		mov	[ioregisters + 0], ax
		inc	ah
		mov	[ioregisters + 2], ax
		inc	ah
		mov	[ioregisters + 4], ax
		inc	ah
		mov	[ioregisters + 6], ax
		sub	ah, 3
		mov	dx, MsgSB16
		jmp	short PrintBoardFound

.board118:	mov	ax, 0x1488
		mov	[ioregisters + 0], ax
		inc	al
		mov	[ioregisters + 2], ax
		inc	al
		mov	[ioregisters + 4], ax
		inc	al
		mov	[ioregisters + 6], ax
		sub	al, 3
		mov	dx, Msg118
		jmp	short PrintBoardFound



; ---- Sub
		; DS:SI += DX:AX
Add_dsi_32:	mov	cx, ax
		or	cx, dx
		je	short .err
		mov	cx, ax
		and	ax, byte 15
		add	si, ax
		shr	cx, 4
		shl	dx, 12
		mov	ax, ds
		add	ax, dx
		add	ax, cx
		mov	ds, ax
		clc
		ret
.err:		stc
		ret

		; S98のチェック
S98Check:	mov	si, dx
		cmp	word [si + S98Header.Magic + 0], 0x3953		; 'S9'
		jne	short .err
		cmp	word [si + S98Header.Magic + 2], 0x3338		; '83'
		jne	short .err

		; PIT タイマー値を計算(いいかげん)
		mov	ax, [si + S98Header.TimerInfo2 + 0]
		mov	dx, [si + S98Header.TimerInfo2 + 2]
		test	ax, ax
		jne	short .num
		test	dx, dx
		jne	short .num
		mov	ax, 1000
.num:		mov	cx, [si + S98Header.TimerInfo]
		test	cx, cx
		jne	short .calcDenom
		mov	cx, 10
.calcDenom:	div	cx
		mov	[cs:cycles_per_sec], ax

		cmp	[si + S98Header.DeviceCount + 0], byte 0
		je	short .setStartPoint
		mov	ax, [si + S98Header.DeviceType + 0]
		cmp	ax, byte 2
		je	short .setStartPoint
		cmp	ax, byte 4
		je	short .setStartPoint
		cmp	ax, byte 6
		jb	short .err
		cmp	ax, byte 9
		ja	short .err
		or	byte [cs:options], OPT_FLAG_OPL

.setStartPoint:	mov	ax, [si + S98Header.DumpData + 0]
		mov	dx, [si + S98Header.DumpData + 2]
		call	Add_dsi_32
		jc	short .err
		mov	[cs:playoff], si
		mov	[cs:playseg], ds
		clc
		ret

.err:		stc
		ret


		; ファイル読み込み
		; cs:bx ファイル名
		; ds:dx 読み込みアドレス
ReadFile:	; ファイル オープン
		push	ds
		mov	ax, 0x3d00
		push	cs
		pop	ds
		push	dx
		mov	dx, bx
		int	0x21
		pop	dx
		pop	ds
		mov	bx, ax
		jc	short .failed
		jmp	short .read

		; アドレス インクリメント
.lp:		mov	ax, ds
		add	ax, 0x800
		mov	ds, ax

		; ファイル リード
.read:		mov	ah, 0x3f
		mov	cx, 0x8000
		int	0x21
		jc	short .readerr
		cmp	ax, cx
		je	short .lp
		; ファイル クローズ
.done:		mov	ah, 0x3e
		int	0x21
		ret

		; エラー
.readerr:	mov	ah, 0x3e
		int	0x21
		stc
.failed:	ret

%include 'BOARDOPN.INC'
%include 'BOARD118.INC'
%include 'SB16.INC'
%include 'OPTIONS.INC'
%include 'STRING.INC'

MsgNonOpt	db	"Usage: S98PLAY Filename.S98", 13, 10, 36
MsgInvalidOpt	db	"Invalid options.", 13, 10, 36
MsgReaderr	db	"Couldn't read file.", 13, 10, 36
MsgInvalidS98	db	"Invalid S98 file.", 13, 10, 36

MsgOPNA		db	"YM2203", 36
MsgSB16		db	"SOUND Blaster 16", 36
Msg118		db	"PC-9801-118", 36
MsgFoundAt	db	" was found at "
MsgFoundPort	db	"0000.", 13, 10, 36


EndOfProgram:
