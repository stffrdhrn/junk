This is to test and issue with sparse where it evaluates
case statements that cannot be evaluated.

```
/* This is an except from:
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/openrisc/include/asm/cmpxchg.h?h=v5.8#n101
 *
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 * arch/openrisc/include/asm/cmpxchg.h:101:29: sparse: sparse: shift too big (32) for type int
 *
 * Example: https://www.mail-archive.com/linux-kernel@vger.kernel.org/msg2256733.html
 */
```
