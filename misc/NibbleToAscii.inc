%ifndef _MISC_NIBBLETOASCII_INC_
%define _MISC_NIBBLETOASCII_INC_

		; Nibble ‚ð Ascii ‚É•ÏŠ·‚·‚é
NibbleToAscii:	and	al, 15
		cmp	al, 10
		jc	short .skip
		add	al, 7
.skip:		add	al, '0'
		ret

%endif	; _MISC_NIBBLETOASCII_INC_
