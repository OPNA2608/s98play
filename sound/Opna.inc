%ifndef _SOUND_OPNA_INC_
%define _SOUND_OPNA_INC_

; OPNA ä÷åW

		; OPNA Çå©Ç¬ÇØÇÈ
OpnaFind:	mov	ah, 1
		call	Port86Enable

		mov	dx, 0x0188
		call	OpnaIsReady
		jc	short .found
		dec	dh
		call	OpnaIsReady
		jc	short .found
		add	dh, 2
		call	OpnaIsReady
		jnc	short .exit
.found:		mov	ax, 0x2983
		call	OpnWriteData
		mov	ax, dx
		stc
.exit:		ret

		; ÇªÇÃÉ|Å[ÉgÇ… OPNA ÇÕÇ†ÇÈ?
OpnaIsReady:	mov	ah, 0xff
		call	OpnReadData
		cmp	al, 0xff
		ret

		; OPN Çå©Ç¬ÇØÇÈ
OpnFind:	mov	dx, 0x0188
		call	OpnWaitAndReadReg
		cmp	al, 0xff
		jnc	short .exit
.found:	mov	ax, 0x2983
		call	OpnWriteData
		mov	ax, dx
		stc
.exit:		ret

%include 'sound/OpnIo.inc'
%include 'sound/Port86.inc'

%endif	; _SOUND_OPNA_INC_
