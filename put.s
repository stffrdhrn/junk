	.file	"put.c"
	.section	.text
	.align 4
	.global	call_puts
	.type	call_puts, @function
call_puts:
	l.movhi	r3, ha(.LC0)
	l.j	puts
	l.addi	r3, r3, lo(.LC0)
	.size	call_puts, .-call_puts
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC0:
	.string	"My test string.\n"
	.ident	"GCC: (GNU) 9.0.0 20180426 (experimental)"
