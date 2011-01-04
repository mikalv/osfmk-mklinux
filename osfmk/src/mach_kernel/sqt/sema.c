/*
 * Copyright 1991-1998 by Open Software Foundation, Inc. 
 *              All Rights Reserved 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 *  
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT, 
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 */
/*
 * MkLinux
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * Copyright (c) 1991 Sequent Computer Systems
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON AND SEQUENT COMPUTER SYSTEMS ALLOW FREE USE OF
 * THIS SOFTWARE IN ITS "AS IS" CONDITION.  CARNEGIE MELLON AND
 * SEQUENT COMPUTER SYSTEMS DISCLAIM ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */

/* CMU_HIST */
/*
 * Revision 2.3  91/07/31  18:03:32  dbg
 * 	Changed copyright.
 * 	[91/07/31            dbg]
 * 
 * Revision 2.2  91/05/08  12:58:36  dbg
 * 	Created.
 * 	[91/04/26  14:56:44  dbg]
 * 
 */

/*
 * Dynix semaphores.
 */
#include <sqt/mutex.h>

#define	sema_lock(sema)		(bit_lock(0, &(sema)->lock))

#define	sema_unlock(sema)	(bit_unlock(0, &(sema)->lock))

/*ARGSUSED*/
init_sema(sema, val, flags, gate)
	register sema_t	*sema;
	int		val;
{
	sema->lock = 0;
	sema->waiting = 0;
	sema->count = val;
}

/*ARGUSED*/
p_sema(sema, pri)
	register sema_t *sema;
	int	pri;
{
	sema_lock(sema);

	if (--sema->count >= 0) {
	    sema_unlock(sema);
	    return;
	}
	sema->waiting = TRUE;
	assert_wait((event_t)sema, FALSE);
	sema_unlock(sema);
	thread_block();
}

p_sema_v_lock(sema, pri, l, spl)
	register sema_t *sema;
	int		pri;
	simple_lock_t	l;
	int		spl;
{
	sema_lock(sema);

	if (--sema->count >= 0) {
	    sema_unlock(sema);
	    v_lock(l,spl);
	    return;
	}
	sema->waiting = TRUE;
	assert_wait((event_t)sema, FALSE);
	sema_unlock(sema);
	v_lock(l,spl);
	thread_block();
}

cp_sema(sema)
	register sema_t	*sema;
{
	sema_lock(sema);
	if (--sema->count >= 0) {
	    sema_unlock(sema);
	    return (TRUE);
	}
	sema_unlock(sema);
	return (FALSE);
}

v_sema(sema)
	register sema_t *sema;
{
	sema_lock(sema);
	if (++sema->count >= 0) {
	    if (sema->waiting) {
		thread_wakeup((event_t)sema);
		sema->waiting = 0;
	    }
	}
	sema_unlock(sema);
}

cv_sema(sema)
	register sema_t *sema;
{
	v_sema(sema);
}

vall_sema(sema)
	register sema_t *sema;
{
	sema_lock(sema);
	while (++sema->count <= 0) {
	    if (sema->waiting) {
		thread_wakeup((event_t)sema);
		sema->waiting = 0;
	    }
	}
	sema_unlock(sema);
}
