%ifndef _SOUND_S98ADDEXT_INC_
%define _SOUND_S98ADDEXT_INC_

; S98 拡張子補完

		; ファイル名補完
S98AddExt:	mov	ah, 0
.lp:		lodsb
		cmp	al, '.'
		je	short .set
		cmp	al, '\'
		jne	short .next
.set:		mov	ah, al
.next:		test	al, al
		jne	short .lp
		cmp	ah, '.'
		je	short .exit
		mov	word [si - 1], 0x532e
		mov	word [si + 1], 0x3839
		mov	[si + 3], al
.exit:		ret

%endif	; _SOUND_S98ADDEXT_INC_
