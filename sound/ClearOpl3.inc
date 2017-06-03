%ifndef _SOUND_CLEAROPL3_INC_
%define _SOUND_CLEAROPL3_INC_

%include 'sound/Port.inc'

SoundClearOpl3:	mov	ax, 0x00bd
		call	SoundSendData
		call	.reset
		mov	ax, 0x0004
		call	SoundSendData

.reset:		mov	ax, 0x00b0
.lp1:		call	SoundSendData
		inc	al
		cmp	al, 0xb9
		jc	short .lp1

		mov	ax, 0x00e0
.lp2:		call	SoundSendData
		inc	al
		mov	dl, al
		and	dl, 7
		cmp	dl, 6
		jc	short .lp2
		add	al, 2
		cmp	al, 0xf6
		jc	short .lp2
		add	bx, byte 4
		ret

%endif	; _SOUND_CLEAROPL3_INC_
