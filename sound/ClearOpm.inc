%ifndef _SOUND_CLEAROPM_INC_
%define _SOUND_CLEAROPM_INC_

%include 'sound/Port.inc'

		; OPM ‚ðƒNƒŠƒA
SoundClearOpm:	mov	ax, 0x0008
.lp1:		call	SoundSendData
		inc	ah
		cmp	ah, 8
		jb	short .lp1
		mov	ax, 0xffe0
.lp2:		call	SoundSendData
		inc	ax
		jne	short .lp2
		add	bx, byte (SoundPort_size * 2)
		ret

%endif	; _SOUND_CLEAROPM_INC_
