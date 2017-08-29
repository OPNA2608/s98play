%ifndef _SOUND_PORT_INC_
%define _SOUND_PORT_INC_

	align	2

%define SoundAddrWait	(SoundSendData.addrwait + 1)	;  4.25us
%define SoundDataWait	(SoundSendData.datawait + 1)	; 20.75us

%ifndef SoundBoardMax
%define SoundBoardMax	1
%endif

SoundPorts	times (SoundBoardMax * 2) dw 0, 0

		struc	SoundPort
.Addr		resw	1
.Data		resw	1
		endstruc

		; OPN/OPL のアドレス/データ セット
SoundSendData:	mov	dx, [SoundPorts + bx + SoundPort.Addr]
		test	dx, dx
		je	short .skip
		out	dx, al
.addrwait:	mov	cx, 7
		call	.wait
		mov	dx, [SoundPorts + bx + SoundPort.Data]
		xchg	al, ah
		out	dx, al
		xchg	al, ah
.datawait:	mov	cx, 35
.wait:		jcxz	.skip
.lp:		out	0x5f, al
		loop	.lp
.skip:		ret


		; ポート ストア
SoundPortSet:	push	ax
		mov	[SoundPorts + bx + SoundPort.Addr], ax
		add	ax, dx
		mov	[SoundPorts + bx + SoundPort.Data], ax
		add	ax, dx
		mov	[SoundPorts + bx + SoundPort_size + SoundPort.Addr], ax
		add	ax, dx
		mov	[SoundPorts + bx + SoundPort_size + SoundPort.Data], ax
		pop	ax
		ret

%endif	; _SOUND_PORT_INC_
