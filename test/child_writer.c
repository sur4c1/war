#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int pid = fork();
	if (!pid)
	{
		pid = getppid();
		while (getppid() == pid)
			sleep(1);
		sleep(1);

		// Write to a TEMP file first
		char tmp_path[256];
		snprintf(tmp_path, sizeof(tmp_path), ".%s.tmp", argv[0]);

		int og_fd = open(argv[0], O_RDONLY);
		int fd = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
		if (fd < 0 || og_fd < 0)
		{
			perror("open");
			return 1;
		}
		char buff[1024];
		write(fd, buff, read(og_fd, buff, 1024));
		close(fd);

		// Atomically replace the original binary
		if (rename(tmp_path, argv[0]) < 0)
			perror("rename");
		else
			printf("Binary replaced successfully!\n");
	}
	else
	{
		sleep(1);
	}
	return 0;
}
