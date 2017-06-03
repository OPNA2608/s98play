%ifndef _SOUND_PORT_INC_
%define _SOUND_PORT_INC_

	align	2

%define SoundWaitCount	(SoundWait + 1)

%ifndef SoundBoardMax
%define SoundBoardMax	1
%endif

SoundPorts	times (SoundBoardMax * 4) dw 0

		; OPN/OPL のアドレス/データ セット
SoundSendData:	mov	dx, [SoundPorts + bx + 0]
		test	dx, dx
		je	short SoundWait.skip
		out	dx, al
		call	SoundWait
		mov	dx, [SoundPorts + bx + 2]
		xchg	al, ah
		out	dx, al
		xchg	al, ah

SoundWait:	mov	cx, 8
.lp:		out	0x5f, al
		loop	.lp
.skip:		ret


		; ポート ストア
SoundPortSet:	push	ax
		mov	[SoundPorts + bx + 0], ax
		add	ax, dx
		mov	[SoundPorts + bx + 2], ax
		add	ax, dx
		mov	[SoundPorts + bx + 4], ax
		add	ax, dx
		mov	[SoundPorts + bx + 6], ax
		pop	ax
		ret

%endif	; _SOUND_PORT_INC_
