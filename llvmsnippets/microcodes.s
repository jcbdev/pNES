	.text
	.intel_syntax noprefix
	.file	"microcodes.c"
	.globl	read                    # -- Begin function read
	.p2align	4, 0x90
	.type	read,@function
read:                                   # @read
# %bb.0:
	movzx	eax, word ptr [rdi + 2062]
	cmp	eax, 8191
	ja	.LBB0_2
# %bb.1:
	and	eax, 2047
	mov	al, byte ptr [rdi + rax]
	mov	byte ptr [rdi + 2060], al
.LBB0_2:
	ret
.Lfunc_end0:
	.size	read, .Lfunc_end0-read
                                        # -- End function
	.globl	readPc                  # -- Begin function readPc
	.p2align	4, 0x90
	.type	readPc,@function
readPc:                                 # @readPc
# %bb.0:
	movzx	eax, word ptr [rdi + 2048]
	mov	word ptr [rdi + 2062], ax
	jmp	read                    # TAILCALL
.Lfunc_end1:
	.size	readPc, .Lfunc_end1-readPc
                                        # -- End function
	.globl	readPcAndInc            # -- Begin function readPcAndInc
	.p2align	4, 0x90
	.type	readPcAndInc,@function
readPcAndInc:                           # @readPcAndInc
# %bb.0:
	movzx	eax, word ptr [rdi + 2048]
	mov	ecx, eax
	add	ecx, 1
	mov	word ptr [rdi + 2048], cx
	mov	word ptr [rdi + 2062], ax
	jmp	read                    # TAILCALL
.Lfunc_end2:
	.size	readPcAndInc, .Lfunc_end2-readPcAndInc
                                        # -- End function
	.globl	testInterrupt           # -- Begin function testInterrupt
	.p2align	4, 0x90
	.type	testInterrupt,@function
testInterrupt:                          # @testInterrupt
# %bb.0:
	mov	al, byte ptr [rdi + 2057]
	mov	cl, byte ptr [rdi + 2081]
	or	cl, byte ptr [rdi + 2080]
	not	al
	and	al, cl
	or	al, byte ptr [rdi + 2082]
	mov	byte ptr [rdi + 2083], al
	ret
.Lfunc_end3:
	.size	testInterrupt, .Lfunc_end3-testInterrupt
                                        # -- End function
	.globl	addClocks               # -- Begin function addClocks
	.p2align	4, 0x90
	.type	addClocks,@function
addClocks:                              # @addClocks
# %bb.0:
	add	dword ptr [rdi + 2064], 3
	ret
.Lfunc_end4:
	.size	addClocks, .Lfunc_end4-addClocks
                                        # -- End function
	.globl	SetFlagI                # -- Begin function SetFlagI
	.p2align	4, 0x90
	.type	SetFlagI,@function
SetFlagI:                               # @SetFlagI
# %bb.0:
	mov	byte ptr [rdi + 2057], 1
	ret
.Lfunc_end5:
	.size	SetFlagI, .Lfunc_end5-SetFlagI
                                        # -- End function
	.globl	ClearFlagD              # -- Begin function ClearFlagD
	.p2align	4, 0x90
	.type	ClearFlagD,@function
ClearFlagD:                             # @ClearFlagD
# %bb.0:
	mov	byte ptr [rdi + 2056], 0
	ret
.Lfunc_end6:
	.size	ClearFlagD, .Lfunc_end6-ClearFlagD
                                        # -- End function
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
# %bb.0:
	push	rax
	mov	word ptr [rip + cpuState+2048], 65
	mov	dword ptr [rip + cpuState+2050], -33619968
	mov	byte ptr [rip + cpuState+2057], 1
	mov	edi, offset cpuState
	call	addClocks
	mov	edi, offset cpuState
	call	readPc
	mov	edi, offset cpuState
	call	read
	mov	edi, offset cpuState
	call	readPcAndInc
	mov	edi, offset cpuState
	call	testInterrupt
	mov	edi, offset cpuState
	call	SetFlagI
	mov	edi, offset cpuState
	call	ClearFlagD
	movzx	eax, byte ptr [rip + cpuState+2060]
	pop	rcx
	ret
.Lfunc_end7:
	.size	main, .Lfunc_end7-main
                                        # -- End function
	.type	cpuState,@object        # @cpuState
	.comm	cpuState,2088,8

	.ident	"clang version 6.0.0-1ubuntu2 (tags/RELEASE_600/final)"
	.section	".note.GNU-stack","",@progbits
