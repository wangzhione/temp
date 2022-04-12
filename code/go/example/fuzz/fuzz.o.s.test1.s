
CMPQ 0x10(R14), SP	
JBE 0xec2		
SUBQ $0x30, SP		
MOVQ BP, 0x28(SP)	
LEAQ 0x28(SP), BP	
MOVQ $0x0, R13		
MOVQ R13, 0x20(SP)	
MOVB $0x0, 0x7(SP)	
MOVQ $0x0, 0x8(SP)	
MOVQ $0x9, 0x8(SP)	
MOVUPS X15, 0x10(SP)	
LEAQ 0(IP), AX		[3:7]R_PCREL:"".test1.func1	
MOVQ AX, 0x10(SP)	
LEAQ 0x8(SP), AX	
MOVQ AX, 0x18(SP)	
LEAQ 0x10(SP), AX	
MOVQ AX, 0x20(SP)	
MOVB $0x0, 0x7(SP)	
MOVQ 0x20(SP), DX	





MOVQ 0x8(SP), AX	
MOVQ 0x28(SP), BP	
ADDQ $0x30, SP		


CALL 0xeb3		[1:5]R_CALL:runtime.deferreturn<1>	
MOVQ 0x8(SP), AX	
MOVQ 0x28(SP), BP	
ADDQ $0x30, SP		

CALL 0xec7		[1:5]R_CALL:runtime.morestack_noctxt	
JMP "".test1(SB)	


MOVQ 0x8(DX), AX	
MOVQ $0x63, 0(AX)	
