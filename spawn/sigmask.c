#include <stdio.h>
#include <signal.h>
#include <stdint.h>

#if 0
#define	SIGINT		2	/* Interactive attention signal.  */
#define	SIGILL		4	/* Illegal instruction.  */
#define	SIGABRT		6	/* Abnormal termination.  */
#define	SIGFPE		8	/* Erroneous arithmetic operation.  */
#define	SIGSEGV		11	/* Invalid access to storage.  */
#define	SIGTERM		15	/* Termination request.  */
#define	SIGHUP		1	/* Hangup.  */
#define	SIGQUIT		3	/* Quit.  */
#define	SIGTRAP		5	/* Trace/breakpoint trap.  */
#define	SIGKILL		9	/* Killed.  */
#define SIGBUS		10	/* Bus error.  */
#define	SIGSYS		12	/* Bad system call.  */
#define	SIGPIPE		13	/* Broken pipe.  */
#define	SIGALRM		14	/* Alarm clock.  */
#define	SIGURG		16	/* Urgent data is available at a socket.  */
#define	SIGSTOP		17	/* Stop, unblockable.  */
#define	SIGTSTP		18	/* Keyboard stop.  */
#define	SIGCONT		19	/* Continue.  */
#define	SIGCHLD		20	/* Child terminated or stopped.  */
#define	SIGTTIN		21	/* Background read from control terminal.  */
#define	SIGTTOU		22	/* Background write to control terminal.  */
#define	SIGPOLL		23	/* Pollable event occurred (System V).  */
#define	SIGXCPU		24	/* CPU time limit exceeded.  */
#define	SIGXFSZ		25	/* File size limit exceeded.  */
#define	SIGVTALRM	26	/* Virtual timer expired.  */
#define	SIGPROF		27	/* Profiling timer expired.  */
#define	SIGUSR1		30	/* User-defined signal 1.  */
#define	SIGUSR2		31	/* User-defined signal 2.  */
#define	SIGWINCH	28	/* Window size change (4.3 BSD, Sun).  */
#endif

#undef SIGRTMIN
#define SIGRTMIN 32
#define SIGRT_1 33
#define SIGRT_2 34
#define SIGRT_3 35
#define SIGRT_4 36
#define SIGRT_5 37
#define SIGRT_6 38
#define SIGRT_7 39
#define SIGRT_8 40
#define SIGRT_9 41
#define SIGRT_10 42
#define SIGRT_11 43
#define SIGRT_12 44
#define SIGRT_13 45
#define SIGRT_14 46
#define SIGRT_15 47
#define SIGRT_16 48
#define SIGRT_17 49
#define SIGRT_18 50
#define SIGRT_19 51
#define SIGRT_20 52
#define SIGRT_21 53
#define SIGRT_22 54
#define SIGRT_23 55
#define SIGRT_24 56
#define SIGRT_25 57
#define SIGRT_26 58
#define SIGRT_27 59
#define SIGRT_28 60
#define SIGRT_29 61
#define SIGRT_30 62
#define SIGRT_31 63
#define SIGRT_32 64

// MASK BEFORE (GOOD BAD SAME)
//      : ~[BUS KILL SEGV STOP RTMIN RT_1 RT_31 RT_32]

// MASK AFTER
// GOOD : [TRAP ABRT USR1 ALRM RT_3 RT_4 RT_6 RT_9 RT_12 RT_13 RT_14 RT_16 RT_17
//         RT_18 RT_19 RT_20 RT_21 RT_22 RT_23 RT_24 RT_25 RT_26 RT_27 RT_28 RT_29
//         RT_30]
//
// BAD  : [HUP INT QUIT FPE PIPE ALRM TERM STKFLT CONT TTIN XCPU PROF WINCH IO PWR
//         SYS]

#define bit(s) (((uint64_t)1) << (SIG##s - 1))

uint64_t before_setmask = ~(bit(BUS) | bit(KILL) | bit(SEGV) | bit(STOP) | bit(RTMIN) | bit(RT_1) | bit(RT_31) | bit(RT_32));
uint64_t after_good_setmask = (bit(TRAP) | bit(ABRT) | bit(USR1) | bit(ALRM) | bit(RT_3) | bit(RT_4) \
                                  | bit(RT_6) | bit(RT_9) | bit(RT_12) | bit(RT_13) | bit(RT_14) | bit(RT_16) \
				  | bit(RT_17) | bit(RT_18) | bit(RT_19) | bit(RT_20) | bit(RT_21) | bit(RT_22) \
                                  | bit(RT_23) | bit(RT_24) | bit(RT_25) | bit(RT_26) | bit(RT_27) | bit(RT_28) \
                                  | bit(RT_29) | bit(RT_30));

uint64_t after_bad_setmask = (bit(HUP) | bit(INT) | bit(QUIT) | bit(FPE) | bit(PIPE) | bit(ALRM) | bit(TERM) \
                                 | bit(STKFLT) | bit(CONT) | bit(TTIN) | bit(XCPU) | bit(PROF) | bit(WINCH) \
                                 | bit(IO) | bit(PWR) | bit(SYS));

int main() {
  printf ("set mask before      : %016lx\n", before_setmask);
  printf ("set mask after (good): %016lx\n", after_good_setmask);
  printf ("set mask after (bad) : %016lx\n", after_bad_setmask);

  return 0;
}
