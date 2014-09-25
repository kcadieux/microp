	AREA text, CODE, READONLY
	EXPORT CrcAssVersion
CrcAssVersion

string RN R0
stringLength RN R1
polynom RN R4
bitCounter RN R6
character RN R7
characterIndex RN R10
remainder RN R11

	MOV polynom, #0x8408 ;can't XOR with more than 255 (constant)
CLOOP
	LDR character, [string] ;ask for one instruction (access pointed data)
	AND character, #0xFF
	EOR remainder, character
	MOV bitCounter, #0 ;

BLOOP
	AND R9, remainder, #0x0001
	CMP R9, #1
	BEQ SHIFTANDXOR
	
	BNE SHIFT
	
SHIFT
	LSR remainder, #1
	B OUTOFIF
	
SHIFTANDXOR
	LSR remainder, #1
	EOR remainder, polynom
	B OUTOFIF
	
OUTOFIF
	ADD bitCounter, #1
	CMP bitCounter, #8
	BEQ BLOOPDONE
	BLT BLOOP
		
BLOOPDONE	
	ADD characterIndex, #1
	ADD string, #1
	CMP characterIndex, stringLength
	BEQ CLOOPDONE
	BLT CLOOP
CLOOPDONE	
	MOV R0, remainder ; Bring the remainder into R0 (output register)
	BX LR ;
	END