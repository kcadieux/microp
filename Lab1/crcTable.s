	AREA text, CODE, READONLY
	EXPORT CrcAssVersionTable
CrcAssVersionTable

string RN R0
stringLength RN R1
table RN R2
polynom RN R4
pointerEnd RN R5
character RN R6
tableValue RN R7
temp RN R8
remainder RN R9
mask RN R10

	PUSH {R1-R10} ; Save the registers value
	MOV mask, #0xffff ; mask to get the 16 rightmost bits
	MOV remainder, #0 ; Initialize remainder at 0 (in case of multiple calls to fct
	ADD pointerEnd, string, stringLength ; End loop condition
	
CLOOP	
	LDRB character, [string], #1 ; Load the value then add 1
	
	AND temp, remainder, #0xff
	EOR temp, character
	LDR tableValue, [table, temp, LSL #1]
	AND tableValue, mask
	EOR remainder, tableValue, remainder, LSR #8
	
	CMP string, pointerEnd 
	BNE CLOOP
	
	MOV string, remainder ; Bring the remainder into R0 (output register)
	
	POP {R1-R10}
	
	BX LR ;
	END