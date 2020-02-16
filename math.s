	.file	"math.c"
	.section	.text
	.global	__udivsi3
	.align 4
	.global	div
	.type	div, @function
div:
	l.addi	r1, r1, -16
	l.sw	8(r1), r2
	l.addi	r2, r1, 16
	l.sw	12(r1), r9
	l.sw	-12(r2), r3
	l.sw	-16(r2), r4
	l.lwz	r4, -16(r2)
	l.lwz	r3, -12(r2)
	l.jal	__udivsi3
	 l.nop

	l.or	r17, r11, r11
	l.or	r11, r17, r17
	l.lwz	r2, 8(r1)
	l.lwz	r9, 12(r1)
	l.addi	r1, r1, 16
	l.jr	r9
	 l.nop

	.size	div, .-div
	.section	.rodata
.LC0:
	.string	"400/3 = %d\n"
	.section	.text
	.align 4
	.global	main
	.type	main, @function
main:
	l.addi	r1, r1, -12
	l.sw	4(r1), r2
	l.addi	r2, r1, 12
	l.sw	8(r1), r9
	l.ori	r4, r0, 3
	l.ori	r3, r0, 400
	l.jal	div
	 l.nop

	l.or	r17, r11, r11
	l.sw	0(r1), r17
	l.movhi	r17, ha(.LC0)
	l.addi	r3, r17, lo(.LC0)
	l.jal	printf
	 l.nop

	l.movhi	r17, hi(0)
	l.or	r11, r17, r17
	l.lwz	r2, 4(r1)
	l.lwz	r9, 8(r1)
	l.addi	r1, r1, 12
	l.jr	r9
	 l.nop

	.size	main, .-main
	.ident	"GCC: (GNU) 9.0.1 20190409 (experimental)"
