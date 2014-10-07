	AREA text, CODE, READONLY
	EXPORT CrcAssVersion
CrcAssVersion

string RN R0
stringLength RN R1
polynom RN R5
bitCounter RN R6
character RN R7
temp RN R9
characterIndex RN R10
remainder RN R11

	PUSH {R1-R12} ; Save the registers value
	MOV characterIndex, #0 ; Initialize char index at 0 (in case of multiple calls to fct)
	MOV remainder, #0 ; Initialize remainder at 0 (in case of multiple calls to fct
	MOV polynom, #0x8408 ; Can't XOR with more than 255 (constant)

CLOOP
	CMP characterIndex, stringLength
	BEQ CLOOPDONE
	ADD characterIndex, #1
	LDRB character, [string], #1 ;
	EOR remainder, character
	AND bitCounter, #0 ; Initialize bitCounter to 0
BLOOP
	CMP bitCounter, #8
	BEQ CLOOP
	AND temp, remainder, #0x0001
	CMP temp, #1
	LSR remainder, #1
	EOREQ remainder, polynom
	ADD bitCounter, #1
	B BLOOP	
	
CLOOPDONE	
	MOV string, remainder ; Bring the remainder into R0 (output register)
	
	POP {R1-R12}
	
	BX LR ;
	END