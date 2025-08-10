%ifndef _SOUND_OPNA_INC_
%define _SOUND_OPNA_INC_

; OPNA 関係

		; OPNA を見つける
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

		; そのポートに OPNA はある?
OpnaIsReady:	mov	ah, 0xff
		call	OpnReadData
		; expected response of a YM2608, according to https://www.webtech.co.jp/company/doc/undocumented_mem/io_sound.txt
		cmp	al, 0x01
		je	short .has2608Value

		clc
		jmp	short .exit

.has2608Value:	stc
.exit:		ret

		; OPN を見つける
OpnFind:	mov	dx, 0x0188
		call	OpnIsReady
		jc	short .found
		dec	dh
		call	OpnIsReady
		jc	short .found
		add	dh, 2
		call	OpnaIsReady
		jnc	short .exit
.found:		mov	ax, dx
.exit:		ret

		; そのポートに OPN はある?
OpnIsReady:	mov	al, 0x00
		call	OpnWaitAndWriteReg

		; register view set to Channel-A Fine Tune
		; check if we can write to it and read back the same value
		mov	ah, 0x99
		mov	al, ah
		add	dl, 2
		call	OpnWaitAndWriteReg
		call	OpnWaitAndReadReg
		sub	dl, 2
		cmp	al, ah
		je	short .hasSameValue

		clc
		jmp	short .exit

.hasSameValue:	stc

.exit:		ret

%include 'sound/OpnIo.inc'
%include 'sound/Port86.inc'

%endif	; _SOUND_OPNA_INC_
