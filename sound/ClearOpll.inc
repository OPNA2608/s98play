%ifndef _SOUND_CLEAROPLL_INC_
%define _SOUND_CLEAROPLL_INC_

%include 'sound/Port.inc'

SoundClearOpll:	mov	ax, 0x000e
		call	SoundSendData
		mov	ax, 0x0020
.lp:		call	SoundSendData
		inc	al
		cmp	al, 0x29
		jc	short .lp
		add	bx, byte SoundPort_size * 2
		ret

%endif	; _SOUND_CLEAROPLL_INC_
