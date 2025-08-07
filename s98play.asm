
	USE16
	CPU	186

OPT_DEF_PORT	equ	0x01
OPT_DEF_WAIT	equ	0x02
OPT_FLAG_SWAIT	equ	0x04
OPT_FLAG_OPNB	equ	0x08
OPT_FLAG_OPN	equ	0x10
OPT_FLAG_OPL	equ	0x20
OPT_FLAG_OPM	equ	0x40
OPT_FLAG_OPLL	equ	0x80

%define SoundBoardMax	4

	ORG	0x100

		jmp	Main

options		db	0

data_addr	dw	0, 0

; ---- メイン

Main:		cld
		mov	[data_addr + 2], cs

		; メモリ アドレス設定
		mov	ax, EndOfProgram + 512
		add	ax, byte 15
		and	ax, byte 0xfff0
		mov	sp, ax
		dec	ax
		shr	ax, 4
		inc	ax
		add	[data_addr + 2], ax

		call	ParseOption
		mov	dx, MsgInvalidOpt
		jc	short .err
		test	bx, bx
		mov	dx, MsgNonOpt
		je	short .err

		; ファイル読み込み
		call	S98Read
		jc	short .err

		; S98 チェック
		push	ds
		lds	si, [data_addr]
		call	S98Prepare
		pop	ds
		jnc	short ChipSelect
		mov	dx, MsgInvalidS98

.err:		mov	ah, 9
		int	0x21
		mov	ax, 0x4c01
		int	0x21

ChipSelect:	call	S98Chip

		; ウェイト セットアップ
SetWait:	test	byte [options], OPT_FLAG_SWAIT
		jne	short .waitset
		call	HasHardwareWait
		jne	short .waited
.waitset:	call	SoundPortWaitSetup
.waited:

		; ボード判定
SetPorts:	test	byte [options], OPT_DEF_PORT
		jne	short Playing
		call	CheckBoards

PrintTitle:	call	S98FindTitle
		mov	ax, [S98TitlAddrH]
		cmp	ax, 0
		jnz	short .hasTitle

		mov	ax, [S98TitlAddrL]
		cmp	ax, 0
		jz	short Playing

.hasTitle:
		call	PrintSongTitle

Playing:	cli
		call	S98Play
		sti

.lp:		hlt
		cmp	byte [S98EndOfData], 0
		jne	short .exit
		mov	ah, 0x06
		mov	dl, 0xff
		int	0x21
		je	short .lp

.exit:		cli
		call	S98Stop
		call	ClearBoards
		call	Ymf297Reset
		sti

		mov	ax, 0x4c00
		int	0x21


; ---- Boards

		; ボード チェック
CheckBoards:	xor	bx, bx
		test	byte [options], OPT_FLAG_OPN
		jne	short CheckOpnBoards
		test	byte [options], OPT_FLAG_OPL
		jne	short CheckOplBoards
		ret

		; OPN ボード チェック
CheckOpnBoards:	call	OpnaFind
		jc	short .foundOpna
		call	OpnFind
		jc	short .foundOpn
		ret
.foundOpna:		mov	dx, 2
		call	SoundPortSet
		mov	dx, MsgOPNA
		jp	PrintBoardFound
.foundOpn:		mov	dx, 2
		call	SoundPortSet
		mov	dx, MsgOPN

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
CheckOplBoards:	call	SB16Find
		jc	short .sb16
		call	Ymf297Find
		jc	short .board118
		ret

.sb16:		mov	dx, 0x100
		call	SoundPortSet
		mov	dx, MsgSB16
		jmp	short PrintBoardFound

.board118:	mov	ax, 0x1488
		mov	dx, 1
		call	SoundPortSet
		mov	dx, Msg118
		jmp	short PrintBoardFound


PrintSongTitle:	mov	ah, 9
		mov	dx, MsgFoundTitleAt
		int	0x21

		mov	di, MsgFoundTitleAddr
		mov	ax, [S98TitlAddrH]
		call	StoreHex16
		mov	ax, [S98TitlAddrL]
		call	StoreHex16
		mov	ah, 9
		mov	dx, MsgFoundTitleAddr
		int	0x21

		mov	ah, 9
		mov	dx, MsgSongTitle
		int	0x21

.keepFetching:	call	S98GetTitleChar
		mov	ah, 2
		int	0x21

		cmp	dl, 0
		jne	short .keepFetching

		mov ah, 9
		mov dx, MsgNewLine
		int 0x21
		ret


		; OPN/OPL をクリア
ClearBoards:	xor	bx, bx
		mov	al, [options]
		test	al, OPT_FLAG_OPN
		jne	short .opn
		test	al, OPT_FLAG_OPL
		jne	short .opl
		test	al, OPT_FLAG_OPM
		jne	short .opm
		test	al, OPT_FLAG_OPLL
		jne	short .opll
		ret

.opn:		jmp	SoundClearOpna
.opl:		jmp	SoundClearOpl3
.opm:		jmp	SoundClearOpm
.opll:		jmp	SoundClearOpll


; ---- Sub

		; S98ロード
S98Read:	call	.open
		jnc	short .opened
		mov	si, bx
		call	S98AddExt
		call	.open
		mov	dx, MsgOpenErr
		jc	short .exit
.opened:	mov	bx, ax

		call	FileGetSize
		add	ax, byte 15
		adc	dx, byte 0
		call	ShrDAx4
		test	dx, dx
		jne	short .toolarge
		mov	dx, [2]
		sub	dx, [data_addr + 2]
		cmp	dx, ax
		jnc	short .read
.toolarge:	mov	dx, MsgTooLargeErr
		stc
		jmp	short .close

.read:		push	ds
		lds	dx, [data_addr]
		call	FileReadSub
		pop	ds
		mov	dx, MsgReadErr

.close:		pushf
		mov	ah, 0x3e
		int	0x21
		popf
.exit:		ret


.open:		mov	ax, 0x3d00
		mov	dx, bx
		int	0x21
		ret


		; S98のチップテスト
S98Chip:	cmp	ax, byte S98OPN
		je	short .opn
		cmp	ax, byte S98OPNA
		je	short .opna
		cmp	ax, byte S98PSG
		je	short .psg
		cmp	ax, byte S98OPM
		je	short .opm
		cmp	ax, byte S98OPLL
		je	short .opll
		cmp	ax, byte S98OPL
		jb	short .exit
		cmp	ax, byte S98OPL3
		jbe	short .opl3
.exit:		ret

.opna:		call	OpnabDefWait
.opn:
.psg:		or	byte [options], OPT_FLAG_OPN
		ret

.opm:		or	byte [options], OPT_FLAG_OPM
		ret

.opll:		or	byte [options], OPT_FLAG_OPLL
		ret

.opl3:		or	byte [options], OPT_FLAG_OPL
		ret


OpnabDefWait:	test	byte [options], OPT_DEF_WAIT
		jne	short .exit
		call	SetOpnabWait
.exit:		ret

%include 'io/HardWait.inc'
%include 'misc/FileGetSize.inc'
%include 'misc/FileReadSub.inc'
%include 'misc/ShrDAx.inc'
%include 'misc/StoreHex16.inc'
%include 'sound/ClearOpl3.inc'
%include 'sound/ClearOpll.inc'
%include 'sound/ClearOpm.inc'
%include 'sound/ClearOpna.inc'
%include 'sound/Opna.inc'
%include 'sound/PortWait.inc'
%include 'sound/S98.inc'
%include 'sound/S98AddExt.inc'
%include 'sound/Sb16.inc'
%include 'sound/Ymf297.inc'
%include 'options.inc'

MsgNonOpt	db	"Usage: S98PLAY Filename.S98", 13, 10, 36
MsgInvalidOpt	db	"Invalid options.", 13, 10, 36
MsgOpenErr	db	"Couldn't open file.", 13, 10, 36
MsgTooLargeErr	db	"File too large.", 13, 10, 36
MsgReadErr	db	"Couldn't read file.", 13, 10, 36
MsgInvalidS98	db	"Invalid S98 file.", 13, 10, 36

MsgOPN		db	"YM2203", 36
MsgOPNA		db	"YM2608", 36
MsgSB16		db	"SOUND Blaster 16", 36
Msg118		db	"PC-9801-118", 36
MsgFoundAt	db	" was found at "
MsgFoundPort	db	"0000.", 13, 10, 36
MsgFoundTitleAt	db	"Found title at offset 0x", '$'
MsgFoundTitleAddr	db "00000000", 0x0d, 0x0a, '$'
MsgSongTitle	db	"Title: ", '$'
MsgNewLine	db	0x0d, 0x0a, '$'

EndOfProgram:
