%ifndef _MISC_SHRDAX_INC_
%define _MISC_SHRDAX_INC_

		; ƒVƒtƒg
ShrDAx4:	call	ShrDAx2
ShrDAx2:	call	ShrDAx
ShrDAx:		sar	dx, 1
		rcr	ax, 1
		ret

%endif	; _MISC_SHRDAX_INC_
