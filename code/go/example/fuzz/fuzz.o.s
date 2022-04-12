TEXT "".main(SB) gofile../home/zhi/temp/code/go/example/fuzz/fuzz.go
  fuzz.go:3		0xdc7			493b6610		CMPQ 0x10(R14), SP	
  fuzz.go:3		0xdcb			7656			JBE 0xe23		
  fuzz.go:3		0xdcd			4883ec20		SUBQ $0x20, SP		
  fuzz.go:3		0xdd1			48896c2418		MOVQ BP, 0x18(SP)	
  fuzz.go:3		0xdd6			488d6c2418		LEAQ 0x18(SP), BP	
  fuzz.go:4		0xddb			e800000000		CALL 0xde0		[1:5]R_CALL:"".test1	
  fuzz.go:4		0xde0			4889442410		MOVQ AX, 0x10(SP)	
  fuzz.go:4		0xde5			6690			NOPW			
  fuzz.go:4		0xde7			e800000000		CALL 0xdec		[1:5]R_CALL:"".test2	
  fuzz.go:4		0xdec			4889442408		MOVQ AX, 0x8(SP)	
  fuzz.go:4		0xdf1			e800000000		CALL 0xdf6		[1:5]R_CALL:runtime.printlock<1>	
  fuzz.go:4		0xdf6			488b442410		MOVQ 0x10(SP), AX	
  fuzz.go:4		0xdfb			e800000000		CALL 0xe00		[1:5]R_CALL:runtime.printint<1>	
  fuzz.go:4		0xe00			e800000000		CALL 0xe05		[1:5]R_CALL:runtime.printsp<1>	
  fuzz.go:4		0xe05			488b442408		MOVQ 0x8(SP), AX	
  fuzz.go:4		0xe0a			e800000000		CALL 0xe0f		[1:5]R_CALL:runtime.printint<1>		
  fuzz.go:4		0xe0f			e800000000		CALL 0xe14		[1:5]R_CALL:runtime.printnl<1>		
  fuzz.go:4		0xe14			e800000000		CALL 0xe19		[1:5]R_CALL:runtime.printunlock<1>	
  fuzz.go:5		0xe19			488b6c2418		MOVQ 0x18(SP), BP	
  fuzz.go:5		0xe1e			4883c420		ADDQ $0x20, SP		
  fuzz.go:5		0xe22			c3			RET			
  fuzz.go:3		0xe23			0f1f4000		NOPL 0(AX)		
  fuzz.go:3		0xe27			e800000000		CALL 0xe2c		[1:5]R_CALL:runtime.morestack_noctxt	
  fuzz.go:3		0xe2c			eb99			JMP "".main(SB)		

TEXT "".test1(SB) gofile../home/zhi/temp/code/go/example/fuzz/fuzz.go
  fuzz.go:7		0xe2e			493b6610		CMPQ 0x10(R14), SP	
  fuzz.go:7		0xe32			0f868a000000		JBE 0xec2		
  fuzz.go:7		0xe38			4883ec30		SUBQ $0x30, SP		
  fuzz.go:7		0xe3c			48896c2428		MOVQ BP, 0x28(SP)	
  fuzz.go:7		0xe41			488d6c2428		LEAQ 0x28(SP), BP	
  fuzz.go:7		0xe46			49c7c500000000		MOVQ $0x0, R13		
  fuzz.go:7		0xe4d			4c896c2420		MOVQ R13, 0x20(SP)	
  fuzz.go:7		0xe52			c644240700		MOVB $0x0, 0x7(SP)	
  fuzz.go:7		0xe57			48c744240800000000	MOVQ $0x0, 0x8(SP)	
  fuzz.go:8		0xe60			48c744240809000000	MOVQ $0x9, 0x8(SP)	
  fuzz.go:9		0xe69			440f117c2410		MOVUPS X15, 0x10(SP)	
  fuzz.go:9		0xe6f			488d0500000000		LEAQ 0(IP), AX		[3:7]R_PCREL:"".test1.func1	
  fuzz.go:9		0xe76			4889442410		MOVQ AX, 0x10(SP)	
  fuzz.go:9		0xe7b			488d442408		LEAQ 0x8(SP), AX	
  fuzz.go:9		0xe80			4889442418		MOVQ AX, 0x18(SP)	
  fuzz.go:9		0xe85			488d442410		LEAQ 0x10(SP), AX	
  fuzz.go:9		0xe8a			4889442420		MOVQ AX, 0x20(SP)	
  fuzz.go:10		0xe8f			c644240700		MOVB $0x0, 0x7(SP)	
  fuzz.go:10		0xe94			488b542420		MOVQ 0x20(SP), DX	
  fuzz.go:10		0xe99			488b02			MOVQ 0(DX), AX		
  fuzz.go:10		0xe9c			ffd0			CALL AX			[0:0]R_CALLIND		
  fuzz.go:10		0xe9e			488b442408		MOVQ 0x8(SP), AX	
  fuzz.go:10		0xea3			488b6c2428		MOVQ 0x28(SP), BP	
  fuzz.go:10		0xea8			4883c430		ADDQ $0x30, SP		
  fuzz.go:10		0xeac			c3			RET			
  fuzz.go:10		0xead			90			NOPL			
  fuzz.go:10		0xeae			e800000000		CALL 0xeb3		[1:5]R_CALL:runtime.deferreturn<1>	
  fuzz.go:10		0xeb3			488b442408		MOVQ 0x8(SP), AX	
  fuzz.go:10		0xeb8			488b6c2428		MOVQ 0x28(SP), BP	
  fuzz.go:10		0xebd			4883c430		ADDQ $0x30, SP		
  fuzz.go:10		0xec1			c3			RET			
  fuzz.go:7		0xec2			e800000000		CALL 0xec7		[1:5]R_CALL:runtime.morestack_noctxt	
  fuzz.go:7		0xec7			e962ffffff		JMP "".test1(SB)	

TEXT "".test1.func1(SB) gofile../home/zhi/temp/code/go/example/fuzz/fuzz.go
  fuzz.go:9		0xecc			488b4208		MOVQ 0x8(DX), AX	
  fuzz.go:9		0xed0			48c70063000000		MOVQ $0x63, 0(AX)	
  fuzz.go:9		0xed7			c3			RET			

TEXT "".test2(SB) gofile../home/zhi/temp/code/go/example/fuzz/fuzz.go
  fuzz.go:13		0xed8			493b6610		CMPQ 0x10(R14), SP	
  fuzz.go:13		0xedc			0f8698000000		JBE 0xf7a		
  fuzz.go:13		0xee2			4883ec38		SUBQ $0x38, SP		
  fuzz.go:13		0xee6			48896c2430		MOVQ BP, 0x30(SP)	
  fuzz.go:13		0xeeb			488d6c2430		LEAQ 0x30(SP), BP	
  fuzz.go:13		0xef0			49c7c500000000		MOVQ $0x0, R13		
  fuzz.go:13		0xef7			4c896c2428		MOVQ R13, 0x28(SP)	
  fuzz.go:13		0xefc			c644240700		MOVB $0x0, 0x7(SP)	
  fuzz.go:13		0xf01			48c744240800000000	MOVQ $0x0, 0x8(SP)	
  fuzz.go:14		0xf0a			48c744241009000000	MOVQ $0x9, 0x10(SP)	
  fuzz.go:15		0xf13			440f117c2418		MOVUPS X15, 0x18(SP)	
  fuzz.go:15		0xf19			488d0500000000		LEAQ 0(IP), AX		[3:7]R_PCREL:"".test2.func1	
  fuzz.go:15		0xf20			4889442418		MOVQ AX, 0x18(SP)	
  fuzz.go:15		0xf25			488d442410		LEAQ 0x10(SP), AX	
  fuzz.go:15		0xf2a			4889442420		MOVQ AX, 0x20(SP)	
  fuzz.go:15		0xf2f			488d442418		LEAQ 0x18(SP), AX	
  fuzz.go:15		0xf34			4889442428		MOVQ AX, 0x28(SP)	
  fuzz.go:15		0xf39			c644240701		MOVB $0x1, 0x7(SP)	
  fuzz.go:16		0xf3e			488b442410		MOVQ 0x10(SP), AX	
  fuzz.go:16		0xf43			4889442408		MOVQ AX, 0x8(SP)	
  fuzz.go:16		0xf48			c644240700		MOVB $0x0, 0x7(SP)	
  fuzz.go:16		0xf4d			488b542428		MOVQ 0x28(SP), DX	
  fuzz.go:16		0xf52			488b02			MOVQ 0(DX), AX		
  fuzz.go:16		0xf55			ffd0			CALL AX			[0:0]R_CALLIND		
  fuzz.go:16		0xf57			488b442408		MOVQ 0x8(SP), AX	
  fuzz.go:16		0xf5c			488b6c2430		MOVQ 0x30(SP), BP	
  fuzz.go:16		0xf61			4883c438		ADDQ $0x38, SP		
  fuzz.go:16		0xf65			c3			RET			
  fuzz.go:16		0xf66			e800000000		CALL 0xf6b		[1:5]R_CALL:runtime.deferreturn<1>	
  fuzz.go:16		0xf6b			488b442408		MOVQ 0x8(SP), AX	
  fuzz.go:16		0xf70			488b6c2430		MOVQ 0x30(SP), BP	
  fuzz.go:16		0xf75			4883c438		ADDQ $0x38, SP		
  fuzz.go:16		0xf79			c3			RET			
  fuzz.go:13		0xf7a			e800000000		CALL 0xf7f		[1:5]R_CALL:runtime.morestack_noctxt	
  fuzz.go:13		0xf7f			e954ffffff		JMP "".test2(SB)	

TEXT "".test2.func1(SB) gofile../home/zhi/temp/code/go/example/fuzz/fuzz.go
  fuzz.go:15		0xf84			488b4208		MOVQ 0x8(DX), AX	
  fuzz.go:15		0xf88			48c70063000000		MOVQ $0x63, 0(AX)	
  fuzz.go:15		0xf8f			c3			RET			
