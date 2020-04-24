#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

char *text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

int main()
{
	int parent_len = strlen(text_to_write) / 2;
	int child_len = strlen(text_to_write) - parent_len;
	int fd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, 00644);
	if (fd == -1) {
		perror("open() failed!");
		exit(EXIT_FAILURE);
	}

	switch (fork()) {
	case 0:
		lseek(fd, parent_len, SEEK_SET);
		if (write(fd, text_to_write + parent_len, child_len) == -1) {
			perror("write() failed in child!");
			exit(EXIT_FAILURE);
		}
		break;

	case -1:
		perror("fork() failed!");
		exit(EXIT_FAILURE);

	default:
		if (write(fd, text_to_write, parent_len) == -1) {
			perror("write() failed in parent!");
			exit(EXIT_FAILURE);
		}

		if (wait(NULL) == -1) {
			perror("wait() failed");
			exit(EXIT_FAILURE);
		}

		close(fd);
		break;
	}

	return 0;
}
