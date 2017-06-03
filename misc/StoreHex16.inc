%ifndef _MISC_STOREHEX16_INC_
%define _MISC_STOREHEX16_INC_

%include 'misc/NibbleToAscii.inc'

		; ES:DI に 16進数文字列をストアする
StoreHex16:	mov	cx, 4
.lp:		rol	ax, 4
		push	ax
		call	NibbleToAscii
		stosb
		pop	ax
		loop	.lp
		ret

%endif	; _MISC_STOREHEX16_INC_
