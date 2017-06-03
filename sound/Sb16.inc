; SB16ŠÖŒW

%ifndef _SOUND_SB16_INC_
%define _SOUND_SB16_INC_

		; SB16 ‚ðŒ©‚Â‚¯‚é
SB16Find:	mov	dx, 0x20d2
.lp1:		call	.check
		jc	short .found
		add	dx, byte 2
		cmp	dx, 0x20e0
		jc	short .lp1
.found:		mov	ax, dx
		ret

.check:		in	al, dx
		cmp	al, 0xff
		je	short .exit
		add	dh, 0x61
		mov	cx, -1
.lp2:		in	al, dx
		test	al, 0x40
		je	short .break
		loop	.lp2
.break:		sub	dh, 0x61
		add	cx, byte -1
.exit:		ret

%endif	; _SOUND_SB16_INC_
