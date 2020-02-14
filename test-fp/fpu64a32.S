	.section .exception_vectors

anchor:
pi:
	.double  3.14159

	.type   e, @object
	.size   e, 8
e:
	.double  2.71828

	.type   large, @object
	.size   large, 8
large:
	.long  0
	.long  0x1234

	.section .text
start_tests:

	/* Test lf.itof.d int to double conversion.  Setting up:
	 *  r11      pointer to data
	 *  r12,r13  pi as double
	 *  r14,r15  e  as double
	 *  r16,r17  a long long
	 */
	l.ori	r11, r0, ha(anchor)
	l.addi	r11, r11, lo(anchor)
	l.lwz	r12, 0(r11)
	l.lwz	r13, 4(r11)

	l.lwz	r14, 8(r11)
	l.lwz	r15, 12(r11)

	l.lwz	r16, 16(r11)
	l.lwz	r18, 20(r11)

	/* Convert the big long to a double. */
	lf.itof.d r16, r16

	/* Convert the double back to a long, it should match before. */
	lf.ftoi.d r16, r16
	lf.itof.d r16, r16

	/* Add and subtract some double values.  */
	lf.add.d r12, r12, r14

	lf.sub.d r12, r12, r14

	/* Multiply and divide double values.  */
	lf.mul.d r12, r13, r12, r13, r14,r16
	lf.mul.d r12, r13, r12, r14, r14,  r16
	lf.mul.d r12, r14, r12,  r14, r14,r16
	lf.mul.d r12, r13, r12,r13, r14,r15
	lf.mul.d r12, r12, r14

	lf.div.d r12,r14, r12,r14, r14,r16

	/* Test lf.sfge.s set flag if r6 >= r10.  */
	lf.sfge.d r12, r14

	/* Test raising an exception by dividing by 0.  */
	l.ori	r2, r2, 0x1
div0:	lf.div.d r2, r12, r0
