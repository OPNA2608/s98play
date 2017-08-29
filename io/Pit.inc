%ifndef _IO_PIT_INC_
%define _IO_PIT_INC_

; PIT

%include 'misc/Cpu.mac'
%include 'io/Pic.inc'

pit_old_intr	dw	0, 0			; ベクタ退避

		; PIT 割込設定
PitInitialize:	mov	al, 0x08		; ベクタ アドレス設定
		call	VectStore
		mov	[pit_old_intr + 0], ax
		mov	[pit_old_intr + 2], dx
		mov	dl, 0x00		; 割り込みマスク解除
		jmp	PicEnable


		; PIT 割り込みを解放
PitTerminate:	push	ds
		mov	al, 0x08
		lds	dx, [pit_old_intr]
		call	VectRestore
		pop	ds
		jc	short .exit

		mov	dl, 0x80		; 割り込みマスク設定
		call	PicEnable
		and	word [pit_old_intr + 0], byte 0
		and	word [pit_old_intr + 2], byte 0
.exit:		ret


		; インターバル タイマ設定
PitSetInterval:	push	ax
		mov	al, 0x36
		out	0x77, al
		pop	ax
		out	0x5f, al
		out	0x71, al
		out	0x5f, al
		xchg	al, ah
		out	0x71, al
		xchg	al, ah
		ret

		; システム クロック モードを得る
PitClockMode:	push	ds
		MOVSEG	ds, 0
		cmp	byte [0x0501], 0x80
		pop	ds
		sbb	ax, ax
		ret

		; システム クロックを得る (0x1e7800 or 0x258000)
PitGetClock:	call	PitClockMode
		xor	dx, dx
		and	ax, 0x8025 - 0x781e
		add	ax, 0x781e
		xchg	al, dl
		ret

%endif	; _IO_PIT_INC_
