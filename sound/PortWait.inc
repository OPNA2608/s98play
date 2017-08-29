%ifndef _SOUND_PORTWAIT_INC_
%define _SOUND_PORTWAIT_INC_

%include 'sound/Port.inc'

		; ê›íË
SoundPortWaitSetup:
		mov	word [SoundSendData.lp + 0], 0x00eb	; jmp $+2
		mov	word [SoundSendData.lp + 2], 0xfee2	; loop $

		mov	ax, [SoundAddrWait]
		call	SoftWaitCount
		mov	[SoundAddrWait], ax
		mov	ax, [SoundDataWait]
		call	SoftWaitCount
		mov	[SoundDataWait], ax
		ret

SetOpnabWait:	mov	word [SoundAddrWait], 4		;  2.128us
		mov	word [SoundDataWait], 17	; 10.392us
		ret

%include 'io/SoftWait.inc'

%endif	; _SOUND_PORTWAIT_INC_
