/*
 * Copyright (c) 2013-2014, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <asm_macros.S>

/* just look like ATF sp loc */
#define ATF_SPINLOCK_KEY     0xA7F5910C

	.globl	spin_lock
	.globl	spin_unlock


func spin_lock
	/* keep lock address in x2, x0 will be used later */
	mov x2, x0
	sevl
l1:	nop
	/* x0 will get the cpuid (position) */
l2:	mrs	x0,	mpidr_el1
	and	x1, x0, #MPIDR_CPU_MASK
	and	x0, x0, #MPIDR_CLUSTER_MASK
	add	x0, x1, x0, LSR #6
	/* x0 will get the address of g_dormant_log_base[cpuid]
		unsigned long g_dormant_log_base = 0;  */
	ldr	x1,	=DORMANT_LOG_BASE
	ldr	x1,	[x1]
	ldr	x1,	[x1]
	/* long, left shift 3 bits*/
	add	x0, x1, x0, LSL #3
	/* we only use x0, x1, x2, so we have to branch here */
	cbz     x1, l3 /* DORMANT_LOG_BASE is not registered */

	/* exclusive load lock value*/
	ldaxr	w1, [x2]
	/* store loaded lock value to dormant log,
		skip if not init. yet */

	str	    w1,	[x0]
	b l4 /* avoid ldaxr again */
l3:	ldaxr	w1, [x2]
l4:	cbnz	w1, l1

	mov_imm x0, ATF_SPINLOCK_KEY
	stxr	w1, w0, [x2]
	cbnz	w1, l2
	ret


func spin_unlock
	stlr	wzr, [x0]
	ret
