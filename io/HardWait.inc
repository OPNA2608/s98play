%ifndef _IO_HARDWAIT_INC_
%define _IO_HARDWAIT_INC_

%include 'misc/Cpu.mac'

		; ハードウェアウェイトあり?
HasHardwareWait:push	ds
		MOVSEG	ds, byte 0
		test	byte [0x458], 0x80
		jne	short .exit
		test	byte [0x45b], 0x80
.exit:		pop	ds
		ret

%endif	; _IO_HARDWAIT_INC_
