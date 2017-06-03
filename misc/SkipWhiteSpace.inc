%ifndef _MISC_SKIPWHITESPACE_INC_
%define _MISC_SKIPWHITESPACE_INC_

		; ホワイト スペースをスキップする
		; DS:SI ポインタ
SkipWhiteSpace:	lodsb
		dec	al
		cmp	al, ' '
		jc	short SkipWhiteSpace
		dec	si
		ret

%endif	; _MISC_SKIPWHITESPACE_INC_
