%ifndef _SOUND_S98HEADER_INC_
%define _SOUND_S98HEADER_INC_

		struc	S98Header
.Magic		resb	3
.FormatVersion	resb	1
.TimerInfo	resw	2
.TimerInfo2	resw	2
.Compressing	resd	1
.TagOffset	resd	1
.DumpData	resw	2
.LoopPoint	resw	2
.DeviceCount	resd	1
.DeviceType	resd	1
.DeviceClock	resd	1
.Pan		resd	1
		resd	1
		endstruc

S98None		equ	0
S98YM2149	equ	1
S98OPN		equ	2
S98OPN2		equ	3
S98OPNA		equ	4
S98OPM		equ	5
S98OPLL		equ	6
S98OPL		equ	7
S98OPL2		equ	8
S98OPL3		equ	9
S98PSG		equ	15
S98DCSG		equ	16

%endif	; _SOUND_S98HEADER_INC_
