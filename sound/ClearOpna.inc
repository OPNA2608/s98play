%ifndef _SOUND_CLEAROPNA_INC_
%define _SOUND_CLEAROPNA_INC_

%include 'sound/Port.inc'

		; OPNA ‚ðƒNƒŠƒA
SoundClearOpna:	mov	ax, 0xbf07
		call	SoundSendData
		mov	ax, 0xff28
.lp1:		inc	ah
		cmp	ah, 3
		je	short .lp1
		call	SoundSendData
		cmp	ah, 6
		jb	short .lp1

		call	.reset
.reset:		mov	ax, 0xff80
.lp2:		call	SoundSendData
		inc	ax
		call	SoundSendData
		inc	ax
		call	SoundSendData
		inc	ax
		inc	ax
		cmp	al, 0x90
		jc	short .lp2
		add	bx, byte 4
		ret

%endif	; _SOUND_CLEAROPNA_INC_
