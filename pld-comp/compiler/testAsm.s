	.section	 __TEXT,__text,regular,pure_instructions
	.build_version macos, 13, 3 sdk_version 13, 3
	.globl	 _main                           ; -- Begin function main
	.p2align	  2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	wzr, [sp, #12]
	b .main_BB_0
.main_BB_0:
	mov w8, #10
	jmp .main_BB_1
.main_BB_1:
	# epilogue
	add	sp, sp, #16
	ret
	.cfi_endproc
										; -- End function
