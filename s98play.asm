
	USE16
	CPU	186

OPT_DEF_PORT	equ	0x01
OPT_DEF_WAIT	equ	0x02
OPT_FLAG_OPN	equ	0x04
OPT_FLAG_OPL	equ	0x08

%define SoundBoardMax	4

	ORG	0x100

		jmp	Main

options		db	0

dataoff		dw	0
dataseg		dw	0

; ---- ���C��

Main:		cld
		; ������ �A�h���X�ݒ�
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

		; �t�@�C���ǂݍ���
		mov	si, bx
		call	S98AddExt

		push	ds
		lds	dx, [dataoff]
		call	FileRead
		pop	ds
		mov	dx, MsgReaderr
		jc	short .err

		; S98 �`�F�b�N
		push	ds
		lds	si, [dataoff]
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

Playing:	cli
		call	S98Play
		sti

.lp:		cmp	byte [cs:S98EndOfData], 0
		jne	short .ed
		hlt
		hlt
		hlt
		hlt
		mov	ah, 0x06
		mov	dl, 0xff
		int	0x21
		je	short .lp

.ed:		cli
		call	S98Stop
		call	ClearBoards
		call	Ymf297Reset
		sti

		mov	ax, 0x4c00
		int	0x21

		; �{�[�h �`�F�b�N
CheckBoards:	xor	bx, bx
		test	byte [options], OPT_FLAG_OPN
		jne	short CheckOpnBoards
		test	byte [options], OPT_FLAG_OPL
		jne	short CheckOplBoards
		ret

		; OPN �{�[�h �`�F�b�N
CheckOpnBoards:	call	OpnaFind
		jc	short .found
		ret
.found:		mov	dx, 2
		call	SoundPortSet
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


		; OPL �{�[�h �`�F�b�N
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


		; OPN/OPL ���N���A
ClearBoards:	xor	bx, bx
		test	byte [options], OPT_FLAG_OPN
		jne	short .opn
		test	byte [options], OPT_FLAG_OPL
		jne	short .opl
		ret

.opn:		jmp	SoundClearOpna
.opl:		jmp	SoundClearOpl3


; ---- Sub

		; S98�̃`�F�b�N
S98Check:	call	S98Prepare
		jc	short .err

		cmp	ax, byte S98OPN
		je	short .opn
		cmp	ax, byte S98OPNA
		je	short .opn
		cmp	ax, byte S98OPL
		jb	short .otherModule
		cmp	ax, byte S98OPL3
		ja	short .otherModule
		or	byte [options], OPT_FLAG_OPL
.otherModule:	clc
		ret

.opn:		or	byte [options], OPT_FLAG_OPN
.err:		ret

MsgNonOpt	db	"Usage: S98PLAY Filename.S98", 13, 10, 36
MsgInvalidOpt	db	"Invalid options.", 13, 10, 36
MsgReaderr	db	"Couldn't read file.", 13, 10, 36
MsgInvalidS98	db	"Invalid S98 file.", 13, 10, 36

MsgOPNA		db	"YM2608", 36
MsgSB16		db	"SOUND Blaster 16", 36
Msg118		db	"PC-9801-118", 36
MsgFoundAt	db	" was found at "
MsgFoundPort	db	"0000.", 13, 10, 36

%include 'misc/FileRead.inc'
%include 'misc/StoreHex16.inc'
%include 'sound/ClearOpl3.inc'
%include 'sound/ClearOpna.inc'
%include 'sound/Opna.inc'
%include 'sound/S98.inc'
%include 'sound/S98AddExt.inc'
%include 'sound/Sb16.inc'
%include 'sound/Ymf297.inc'
%include 'options.inc'

EndOfProgram:
