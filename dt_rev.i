VERSION		EQU	1
REVISION	EQU	120
DATE	MACRO
		dc.b	'15.10.93'
	ENDM
VERS	MACRO
		dc.b	'dt 1.120'
	ENDM
VSTRING	MACRO
		dc.b	'dt 1.120 (15.10.93)',13,10,0
	ENDM
VERSTAG	MACRO
		dc.b	0,'$VER: dt 1.120 (15.10.93)',0
	ENDM
