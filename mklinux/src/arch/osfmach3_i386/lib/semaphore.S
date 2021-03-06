/*
 * Copyright (c) 1991-1998 Open Software Foundation, Inc. 
 *  
 * 
 */
/*
 * MkLinux
 */

/*
 *  linux/arch/i386/lib/semaphore.S
 *
 *  Copyright (C) 1996  Linus Torvalds
 */

#include <linux/linkage.h>

/*
 * "down_failed" is called with the eventual return address
 * in %eax, and the address of the semaphore in %ecx. We need
 * to increment the number of waiters on the semaphore,
 * call "__down()", and then eventually return to try again.
 */
ENTRY(down_failed)
	pushl %eax
	pushl %ecx
	call SYMBOL_NAME(__down)
	popl %ecx
	ret

ENTRY(up_wakeup)
	pushl %eax
	pushl %ecx
	call SYMBOL_NAME(__up)
	popl %ecx
	ret

ENTRY(down_failed_interruptible)
	pushl %eax
	pushl %ecx
	call SYMBOL_NAME(__down_interruptible)
	popl %ecx
	ret
