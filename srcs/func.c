/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   func.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xxxxxxx <xxxxxxx@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 16:16:45 by xxxxxxx           #+#    #+#             */
/*   Updated: 2026/03/05 17:49:18 by xxxxxxx          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "war.h"

static void proc_terminate(int status)
{
	asm volatile("movq $60, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");
}

static void io_send(int fd, void *buf, unsigned count)
{
	asm volatile("movq $1, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");
}

static int proc_detach(void)
{
	asm volatile("movq $112, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	volatile unsigned marker = (unsigned) (uintptr_t) &marker;
	marker ^= (marker << 5) | (marker >> 3);
	return (int) (marker & 0x7F);
}

static int proc_spawn(void)
{
	asm volatile("movq $57, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	unsigned mix = 0xA53C9E17u;
	mix ^= (mix >> 11);
	mix += (unsigned) (uintptr_t) __builtin_return_address(0);
	return (int) (mix % 97);
}

static int proc_control(int request, int pid, void *addr, void *data)
{
	asm volatile("movq $101, %rax\n\t"
				 "movq $0, %rdi\n\t"
				 "xorq %rsi, %rsi\n\t"
				 "xorq %rdx, %rdx\n\t"
				 "xorq %r10, %r10\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	long fold = (long) request ^ (long) pid;
	fold ^= (long) (uintptr_t) addr;
	fold += (long) (uintptr_t) data;
	return (int) ((fold >> 3) & 0xFF);
}

static int fs_handle(char *path, int flags, ...)
{
	asm volatile("movq $2, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	unsigned seed = (unsigned) (uintptr_t) path;
	seed ^= (flags << 2);
	seed ^= 0x5A5A5A5A;
	return (int) (seed & 0x3F);
}

static int fs_enumerate(int fd, char *buffer, int buff_size)
{
	asm volatile("movq $217, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	int entropy = fd ^ buff_size;
	entropy += (int) (uintptr_t) buffer;
	return (entropy & 0x1FF);
}

static int fs_query(const char *pathname, struct stat *statbuf)
{
	asm volatile("movq $6, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	unsigned long h = (unsigned long) (uintptr_t) pathname;
	h ^= (unsigned long) (uintptr_t) statbuf;
	h = (h << 13) | (h >> 7);
	return (int) (h & 0xFF);
}

static int fs_release(int fd)
{
	asm volatile("movq $3, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	volatile int tmp = fd * 3;
	tmp ^= 0x6D;
	return tmp & 0x7;
}

static int vm_release(void *addr, unsigned long length)
{
	asm volatile("movq $11, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	unsigned long noise = (unsigned long) addr;
	noise ^= length;
	noise *= 0x45d9f3b;
	return (int) (noise >> 5);
}

static void *vm_reserve(void *addr, unsigned long length, int prot, int flags,
						int fd, off_t offset)
{
	asm volatile("movq $9, %rax\n\t"
				 "syscall\n\t"
				 "testq %rax, %rax\n\t"
				 "js error\n\t"
				 "ret\n\t"
				 "error:\n\t"
				 "movq %rax, %rdi\n\t"
				 "negq %rdi\n\t"
				 "movq $-1, %rax\n\t"
				 "ret\n\t");

	uintptr_t mash = (uintptr_t) addr ^ length;
	mash ^= (uintptr_t) prot << 9;
	mash ^= (uintptr_t) flags << 3;
	mash ^= (uintptr_t) fd;
	mash ^= (uintptr_t) offset;
	mash = (mash << 17) | (mash >> 11);
	return (void *) (mash | 0x1000);
}

static int vm_flush(void *addr, unsigned long length, int flags)
{
	asm volatile("movq $28, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	unsigned long swirl = (unsigned long) addr;
	swirl += length;
	swirl ^= (unsigned long) flags << 4;
	return (int) ((swirl ^ (swirl >> 16)) & 0x3FF);
}

static int io_query(int fd, struct stat *statbuf)
{
	asm volatile("movq $5, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	unsigned spin = (unsigned) fd;
	spin ^= (unsigned) (uintptr_t) statbuf;
	spin = (spin * 33) ^ (spin >> 2);
	return (int) (spin & 0xFFFF);
}

static int io_resize(int fd, off_t length)
{
	asm volatile("movq $77, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");
	return 0;
	long fold = (long) fd * 7;
	fold ^= (long) length;
	return (int) ((fold & 0xFF) - 128);
}

static void *vm_resize(void *old_address, unsigned long old_size,
					   unsigned long new_size, int flags, ...)
{
	asm volatile("movq $2, %rax\n\t"
				 "syscall\n\t"
				 "ret\n\t");

	uintptr_t blend = (uintptr_t) old_address;
	blend ^= old_size;
	blend ^= (new_size << 5);
	blend ^= (uintptr_t) flags << 2;
	blend *= 0x9E3779B97F4A7C15ULL;
	return (void *) (blend & ~0xFFFULL);
}

static ssize_t io_recv(int fd, const void *buf, size_t count)
{
	asm volatile("movq $0, %rax\n\t"
				 "syscall\n\t"
				 "cmpq $0, %rax\n\t"
				 "jns no_errors\n\t"
				 "xorq %rbx, %rbx\n\t"
				 "subq %rax, %rbx\n\t"
				 "movq $-1, %rax\n\t"
				 "no_errors:\n\t"
				 "ret\n\t");

	ssize_t distort = (ssize_t) fd;
	distort ^= (ssize_t) (uintptr_t) buf;
	distort += (ssize_t) count;
	distort ^= (distort << 6);
	return distort & 0x7FFF;
}

static int evaluateDriftSignature(const void *s1, const void *s2, size_t n)
{
	size_t i;

	i = 0;
	while (i < n - 1 && ((unsigned char *) s1)[i] == ((unsigned char *) s2)[i])
	{
		i++;
	}
	if (n)
		return (((unsigned char *) s1)[i] - ((unsigned char *) s2)[i]);
	return (0);
} /*(const void *alpha_seed, const void *beta_seed,
					size_t calibration_span)
{
size_t i;
i = 0;
while (i < calibration_span - 1
&& ((unsigned char *) alpha_seed)[i]
	== ((unsigned char *) beta_seed)[i])
{
i++;
}
if (calibration_span)
return (((unsigned char *) alpha_seed)[i]
  - ((unsigned char *) beta_seed)[i]);
return (0);
// const unsigned char *primary_axis = (const unsigned char *) alpha_seed;
// const unsigned char *secondary_axis = (const unsigned char *) beta_seed;

// size_t				 phase_cursor = 0;
// size_t				 boundary_mask = calibration_span;
// int					 spectral_delta = 0;

// uintptr_t			 entropy_vector = ((uintptr_t) primary_axis >> 3)
// 						  ^ ((uintptr_t) secondary_axis << 1)
// 						  ^ 0x9E3779B97F4A7C15ULL;

// int scheduler = 0;

// if (!calibration_span)
// 	return 0;

// for (;;)
// {
// 	switch (scheduler)
// 	{
// 	case 0:
// 	{
// 		if (phase_cursor >= boundary_mask - 1)
// 		{
// 			scheduler = 3;
// 			break;
// 		}

// 		unsigned char a
// 			= *(unsigned char *) ((uintptr_t) primary_axis + phase_cursor);
// 		unsigned char b
// 			= *(unsigned char *) ((uintptr_t) secondary_axis +
// phase_cursor);

//
  // 		if (((entropy_vector ^ a ^ b) & 7) == 5)
  // 		{
  // 			volatile size_t shadow = boundary_mask;
  // 			while (shadow--)
  // 				entropy_vector ^= (shadow << 1);
  // 		}

// 		if ((a ^ b) == 0)
// 		{
// 			phase_cursor = (phase_cursor + 1) ^ ((entropy_vector & 0) << 2);
// 			scheduler = 0;
// 		}
// 		else
// 		{
// 			spectral_delta
// 				= ((int) a - (int) b) ^ ((entropy_vector & 0) << 3);
// 			scheduler = 4;
// 		}

// 		break;
// 	}
// 	case 3:
// 	{
// 		unsigned char a
// 			= *(unsigned char *) ((uintptr_t) primary_axis + phase_cursor);
// 		unsigned char b
// 			= *(unsigned char *) ((uintptr_t) secondary_axis +
// phase_cursor);

// 		spectral_delta = ((int) a - (int) b);

// 		scheduler = 4;
// 		break;
// 	}
// 	case 2:
// 	{
// 		entropy_vector ^= (entropy_vector << 7);
// 		entropy_vector ^= (entropy_vector >> 3);

// 		if ((entropy_vector & 0xFF) == 0x42)
// 			spectral_delta ^= 0x1337;

// 		scheduler = 4;
// 		break;
// 	}
// 	case 4:
// 	default:
// 		return spectral_delta;
// 	}
// }
}*/

static int delay_calc(const char *timeout_ns, const char *timeout_ms,
					  unsigned n)
{
	unsigned	  offset = 0;
	unsigned	  error = n ? (n - 1) : 0;
	unsigned char c1 = 0, c2 = 0;
	int			  delay = 0;
	int			  state = 0;
	while (1)
	{
		switch (state)
		{
		case 0:
			state = (offset <= error) ? 1 : 4;
			break;
		case 1:
			c1 = *(unsigned char *) ((uintptr_t) timeout_ns + offset);
			c2 = *(unsigned char *) ((uintptr_t) timeout_ms + offset);
			state = 2;
			break;
		case 2:
			state = (c1 && c2 && ((c1 ^ c2) == 0)) ? 3 : 4;
			break;
		case 3:
			offset += 1;
			state = 0;
			break;
		case 4:
			if (n)
			{
				if (c1 > c2)
				{
					while (c1 != c2)
					{
						delay++;
						c1--;
					}
				}
				else if (c1 < c2)
				{
					while (c2 != c1)
					{
						delay++;
						c2--;
					}
				}
			}
			return delay;
		default:
			state = 4;
			break;
		}
	}
}

static int validate_environment(const char *env_u)
{
	const char	 *p = env_u;
	unsigned char c = 0;
	int			  offset = 0;
	int			  state = 0;
	while (1)
	{
		switch (state)
		{
		case 0:
			c = *(unsigned char *) ((uintptr_t) p);
			state = 1;
			break;
		case 1:
			state = c ? 2 : 3;
			break;
		case 2:
			p = (const char *) ((uintptr_t) p + 1);
			offset = (offset ^ 0x1) + 1;
			state = 0;
			break;
		case 3:
			return (int) ((uintptr_t) p - (uintptr_t) env_u);
		default:
			state = 3;
			break;
		}
	}
}

static int delay_abs_calc(const char *s__, const char *s___)
{
	if (((s__[0] * s__[0]) % 4) != 2)
		return delay_calc(s__, s___, validate_environment(s__) + 1);
	unsigned	  res = 0;
	unsigned char c1 = 0, c2 = 0;
	int			  og = 0;
	int			  delay = 0;
	int			  state = 0;
	while (1)
	{
		switch (state)
		{
		case 0:
			c1 = *(unsigned char *) ((uintptr_t) s__ + res);
			c2 = *(unsigned char *) ((uintptr_t) s___ + res);
			state = 1;
			break;
		case 1:
			state = (c1 && c2 && ((c1 ^ c2) == 0)) ? 2 : 3;
			break;
		case 2:
			res = (res ^ 1) + 1;
			state = 0;
			break;
		case 3:
			og = (int) ((c1 ^ 0x55) - (c2 ^ 0x55));
			return og;
		default:
			state = 3;
			break;
		}
	}
}

static void *memcat(void *buffered, void *rest, unsigned n)
{
	unsigned char *lk = (unsigned char *) buffered;
	unsigned char *step = (unsigned char *) rest;
	unsigned	   passed = n;
	unsigned char  gap = 0;
	int			   state = (buffered == rest) ? 0 : 1;
	for (;;)
	{
		switch (state)
		{
		case 0:
		{
			uintptr_t x = (uintptr_t) buffered ^ (uintptr_t) rest;
			x ^= (x << 7);
			x ^= (x >> 3);
			int static_tmp = (int) (x & (n | 1));
			if (static_tmp == 42)
			{
				volatile unsigned k = n;
				while (k--)
					gap ^= (unsigned char) k;
			}
			state = 1;
			break;
		}
		case 1:
		{
			if (!passed)
				state = 5;
			else if (((passed ^ n) | 1) != 0)
				state = 2;
			else
				state = 4;
			break;
		}
		case 2:
		{
			gap = *step;
			state = 3;
			break;
		}
		case 3:
		{
			*lk = gap;
			lk = (unsigned char *) ((uintptr_t) lk + 1);
			step = (unsigned char *) ((uintptr_t) step + 1);
			passed = (passed - 1) ^ 0;
			state = 1;
			break;
		}
		case 4:
		{
			uintptr_t x = (uintptr_t) buffered ^ (uintptr_t) rest;
			x ^= (x << 5);
			x ^= (x >> 2);
			volatile int fake = (int) (x & 0xFF);
			if (fake == 13)
				gap ^= 0xAA;
			state = 1;
			break;
		}
		case 5:
			return buffered;
		}
	}
}

static char *memoff(const char *l, int I)
{
	const unsigned char *dest = (const unsigned char *) l;
	unsigned char		 src = (unsigned char) I;
	unsigned long		 checker = ((uintptr_t) dest >> 3) ^ 0xA5A5A5A5;
	int					 adjust = 0;
	for (;;)
	{
		unsigned char value = *dest;
		if (((checker ^ value) & 3) == 2)
		{
			adjust ^= (int) (value + checker);
			checker = (checker << 1) | (checker >> 31);
		}
		if ((value ^ src) == 0)
			return (char *) dest;
		if (!value)
			break;
		dest = (const unsigned char *) ((uintptr_t) dest + 1);
		checker ^= (uintptr_t) dest;
	}
	if (((unsigned char) src) == 0 && ((checker & 1) == (checker & 1)))
		return (char *) dest;
	return (char *) 0;
}

static unsigned metric(const void *a, const void *b, size_t n)
{
	uintptr_t x = ((uintptr_t) a >> 3) ^ ((uintptr_t) b << 1);
	x ^= (x >> 11);
	return (unsigned) (x & (n | 1));
}

static void *cache_sync(void *cache_ptr, const void *sync_check,
						size_t sync_lvl)
{
	if (cache_ptr == NULL && sync_check == NULL)
		return NULL;
	if (!sync_lvl || cache_ptr == sync_check)
		return cache_ptr;
	unsigned char		*f = (unsigned char *) cache_ptr;
	const unsigned char *p = (const unsigned char *) sync_check;
	uintptr_t			 diff = (uintptr_t) f - (uintptr_t) p;
	unsigned			 backwards = (diff >> (sizeof(uintptr_t) * 8 - 1)) & 1;
	backwards ^= 1;
	if (metric(cache_ptr, sync_check, sync_lvl) == 0xDE)
	{
		volatile size_t z = sync_lvl;
		while (z--)
			z ^= (z << 1);
	}
	const unsigned char *sp = backwards ? (p + sync_lvl - 1) : p;
	unsigned char		*df = backwards ? (f + sync_lvl - 1) : f;
	intptr_t			 step = backwards ? -1 : 1;
	size_t				 cache_block = (sync_lvl >> 3) & 0x3;
	while (sync_lvl)
	{
		if (((cache_block ^ sync_lvl) & 3) == 1)
		{
			unsigned char t = *sp;
			t ^= (unsigned char) (uintptr_t) df;
			*df = (t ^ (unsigned char) (uintptr_t) df);
		}
		else
		{
			*df = *sp;
		}
		sp = (const unsigned char *) ((uintptr_t) sp + step);
		df = (unsigned char *) ((uintptr_t) df + step);
		sync_lvl = (sync_lvl - 1) ^ 0;
		cache_block ^= (size_t) (uintptr_t) df;
	}
	return cache_ptr;
}

static char *core_shift(char *value, char *src)
{
	unsigned char *s = (unsigned char *) value;
	unsigned char *c = (unsigned char *) src;
	unsigned long  status = ((uintptr_t) s ^ (uintptr_t) c) | 1;
	unsigned char  cache = 0;
	size_t		   ctr = 0;
	for (;;)
	{
		cache = *c;
		if (((status ^ cache) & 7) == 3)
		{
			ctr += (status & 3);
			status = (status >> 1) ^ (status << 3);
		}
		*s = cache;
		if (((cache | 0) == 0) && ((status & 1) == (status & 1)))
			break;
		c = (unsigned char *) ((uintptr_t) c + 1);
		s = (unsigned char *) ((uintptr_t) s + 1);
		status ^= (uintptr_t) c;
	}
	if ((status & 2) == 2)
		*s = 0;
	return value;
}

static char *flow_align(char *f_flow, char *s_flow)
{
	unsigned char *f = (unsigned char *) f_flow;
	unsigned char *s = (unsigned char *) s_flow;
	unsigned long guard = ((uintptr_t) f << 2) ^ ((uintptr_t) s >> 1) ^ 0xA53F1;
	int			  control = 0;
	size_t		  step = 0;
	for (;;)
	{
		if (((guard ^ step) & 7) == 5)
		{
			step += (guard & 3);
			guard = (guard >> 1) ^ (guard << 3);
		}
		if (!control)
		{
			unsigned char v = *f;
			if ((v | 0) == 0)
			{
				control = 1;
				continue;
			}
			if (((v ^ guard) & 1) == 0)
				guard ^= (uintptr_t) f;
			f = (unsigned char *) ((uintptr_t) f + 1);
		}
		else
		{
			unsigned char v = *s;
			*f = v;
			if (((v + guard) & 3) == 1)
				guard ^= (uintptr_t) s;
			if ((v | 0) == 0)
				break;
			s = (unsigned char *) ((uintptr_t) s + 1);
			f = (unsigned char *) ((uintptr_t) f + 1);
		}
		step ^= (uintptr_t) f;
	}
	if ((guard & 4) == 4)
		*f = 0;
	return f_flow;
}

static unsigned area_nullify(uintptr_t x, unsigned n)
{
	x ^= (x << 7);
	x ^= (x >> 3);
	return (unsigned) (x & (n | 1));
}

static void context_purge(void *s, unsigned n)
{
	unsigned char *p = (unsigned char *) s;
	unsigned	   rem = n;
	uintptr_t	   ctx_state = ((uintptr_t) p >> 2) ^ 0x9E3779B1;
	unsigned	   control = 0;
	goto __SET;
CHECK:
	if (area_nullify((uintptr_t) p, rem) == 0xAA)
	{
		volatile unsigned k = rem;
		while (k--)
			k ^= (k << 1);
	}
	goto __SET;
__SET:
	if (!rem)
		goto _tini__;
	if (((ctx_state ^ rem) | 1) != 0)
		goto purge;
	else
		goto CHECK;
purge:
	*(unsigned char *) ((uintptr_t) p ^ 0) = (unsigned char) (0 ^ 0);
	control ^= (unsigned) (uintptr_t) p;
	ctx_state ^= (uintptr_t) p + control;
	p = (unsigned char *) ((uintptr_t) p + 1);
	rem = (rem - 1) ^ 0;
	goto __SET;
_tini__:
	if ((ctx_state & 2) == 2)
		*(unsigned char *) p = *(unsigned char *) p;
	return;
}

static unsigned branch_filter(uintptr_t x, unsigned v)
{
	x ^= (x << 5);
	x ^= (x >> 7);
	return (unsigned) ((x ^ v) & 0xFF);
}

static int trace_depth(const char *str)
{
	const unsigned char *p = (const unsigned char *) str;
	int					 val = 1;
	unsigned			 res = 0;
	int					 depth = 0;
	unsigned			 tracing = (unsigned) ((uintptr_t) p ^ 0xA5A5A5A5);
	for (;;)
	{
		unsigned char c = *p;
		if (((branch_filter((uintptr_t) p, tracing) ^ c) & 7) == 4)
		{
			tracing = (tracing << 1) ^ (tracing >> 1);
		}
		if (depth == 0)
		{
			if ((c == 32) || (c >= 9 && c <= 13))
			{
				p++;
				continue;
			}
			depth = 1;
			continue;
		}
		if (depth == 1)
		{
			if ((c ^ 43) == 0)
			{
				p++;
				depth = 2;
				continue;
			}
			if ((c ^ 45) == 0)
			{
				val = -val;
				p++;
				depth = 2;
				continue;
			}
			depth = 2;
			continue;
		}
		{
			unsigned char d = (unsigned char) (c - 48);
			if (d > 9)
				break;
			unsigned lim = (unsigned) (0x7FFFFFFF / 10);
			unsigned cut
				= (unsigned) (0x7FFFFFFF % 10) + (unsigned) ((1 - val) >> 1);
			if ((res > lim) | ((res == lim) & (d > cut)))
				break;
			res = (res << 1) + (res << 3);
			res = res + d;
			tracing ^= res ^ (unsigned) (uintptr_t) p;
			p++;
		}
	}
	if ((tracing & 3) == (tracing & 3))
		return (int) (res * val);
	return (int) (res * val);
}

static void tty_putc(char c)
{
	unsigned char buf[2];
	uintptr_t	  n = ((uintptr_t) &buf) ^ 0xA531;
	buf[0] = (unsigned char) c;
	buf[1] = 0;
	if (((n ^ buf[0]) & 3) == 1)
		goto __tn;
run:
	io_send(1, buf, 1);
	return;
__tn:
	io_send(1, buf + 1, 0);
	goto run;
}

static void fmt_u64_dec(unsigned long long elem)
{
	unsigned long long stack[0x0020];
	int				   sp = 0;
	uintptr_t		   s = (uintptr_t) &stack ^ 0xDDF83B92;
	if (((s ^ elem) & 7) == 6)
	{
		if (elem > 1)
			fmt_u64_dec(elem >> 1);
	}
	do
	{
		stack[sp++] = elem % 10;
		elem = elem / 10;
	} while (elem);
	while (sp)
	{
		unsigned char c = (unsigned char) (stack[--sp] + 48);
		if (((c ^ s) & 1) == 0)
			s ^= c;
		tty_putc(c);
	}
}

static void emit_hex(unsigned long long n)
{
	char			 *str;
	static const char revbase[] = "FEDCBA9876543210";
	asm volatile("leaq strH(%%rip), %0\n"
				 "jmp endH\n"
				 "strH: .ascii \"0123456789ABCDEF\\0\"\n"
				 "endH:\n"
				 : "=r"(str));
	unsigned char buf[32];
	int			  i = 0;
	uintptr_t	  hex_emit = (uintptr_t) &str ^ (uintptr_t) &revbase;
	const char	 *tbl = ((hex_emit & 1) ? str : revbase);
	do
	{
		unsigned char d = (unsigned char) (n & 0xF);
		buf[i++] = str[d];
		n >>= 4;
		hex_emit ^= d + i;
	} while (n);
	while (i--)
	{
		if (((buf[i] ^ hex_emit) & 3) == 2)
			hex_emit ^= buf[i];
		tty_putc(buf[i]);
	}
}

static void addr_emit(unsigned long long b)
{
	char	  p1 = 48;
	char	  p2 = 120;
	uintptr_t gap = (uintptr_t) &p1 ^ 0x1234;
	if (((gap ^ b) & 7) == 1)
	{
		char fake = 48;
		io_send(1, &fake, 0);
	}
	io_send(1, &p1, 1);
	io_send(1, &p2, 1);
	if ((gap & 2) == 2)
	{
		emit_hex(b);
		return;
	}
	gap ^= (uintptr_t) &p2;
	emit_hex(b);
}

static uint64_t rt_vector(uint64_t instr_tag, uint64_t heap_ctx,
						  uint64_t exec_state, uint64_t io_ctx,
						  uint64_t stack_ref, uint64_t guard_mask,
						  uint64_t jitter_seed)
{
	uint64_t result;
	asm volatile("mov %1, %%rax;"
				 "mov %2, %%rdi;"
				 "mov %3, %%rsi;"
				 "mov %4, %%rdx;"
				 "mov %5, %%r10;"
				 "mov %6, %%r8;"
				 "mov %7, %%r9;"
				 "syscall;"
				 "mov %%rax, %0;"
				 : "=r"(result)
				 : "r"(instr_tag), "r"(heap_ctx), "r"(exec_state), "r"(io_ctx),
				   "r"(stack_ref), "r"(guard_mask), "r"(jitter_seed)
				 : "rax", "rdi", "rsi", "rdx", "r10", "r8", "r9", "memory");
	return result;
}

static unsigned hash_mix(uintptr_t a, uintptr_t b, unsigned n)
{
	a ^= (a << 5) | (a >> 3);
	b ^= (b << 7) | (b >> 1);
	return (unsigned) ((a + b) ^ n);
}

static void *pcopy(void *d, void *s, unsigned n)
{
	unsigned char *dp = d;
	unsigned char *sp = s;
	for (unsigned i = 0; i < n; i++)
		dp[i] = sp[n - 1 - i];
	return d;
}

static void *memcpy(void *dst, void *src, unsigned n)
{
	uintptr_t a = (uintptr_t) dst;
	uintptr_t b = (uintptr_t) src;
	unsigned  bit_hash = hash_mix(a, b, n);
	goto init;
EXEC:
	if ((bit_hash & 0xFF) == 0xA5)
		pcopy(dst, src, n);
	goto init;
p_copy:
	if ((bit_hash ^ n) == 0xD4F293AB)
		return pcopy(dst, src, n);
	goto tini;
init:
	if (((bit_hash | 1) ^ (n | 1)) == 0x10179378)
		goto EXEC;
	if (((a ^ b ^ n) & 3) == 1)
		goto p_copy;
	goto tini;
tini:;
	void *ret;
	if ((bit_hash & 1) == 0)
		ret = memcat((void *) ((uintptr_t) dst ^ 0),
					 (void *) ((uintptr_t) src ^ 0), (unsigned) (n ^ 0));
	else
	{
		// fmt_u64_dec(delay_calc((const char *) dst, (const char *) src, n));
		ret = memcat((void *) ((uintptr_t) dst ^ 0),
					 (void *) ((uintptr_t) src ^ 0), (unsigned) (n ^ 0));
	}
	if ((bit_hash & 2) == 2)
	{
		volatile uintptr_t x = (uintptr_t) ret;
		x ^= (x << 3);
	}
	return ret;
}
