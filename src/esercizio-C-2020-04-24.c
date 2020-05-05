#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <errno.h>


int main(int argc, char *argv[]) {

	char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";
	char * file_name = "output.txt";
	int text_to_write_len;

	int fd = open(file_name,
					  O_CREAT | O_TRUNC | O_WRONLY,
					  S_IRUSR | S_IWUSR);

	if (fd == -1) {

			perror("open()");

			exit(EXIT_FAILURE);
		}

	text_to_write_len = strlen(text_to_write) + 1;

	int res = write(fd, text_to_write, text_to_write_len / 2);

	if (res == -1) {
		perror("write()");
		exit(EXIT_FAILURE);
	}

	pid_t child_pid;

	child_pid = fork();

	if (child_pid == 0){

		int res2 = write(fd, text_to_write + res, text_to_write_len - res);

		if (res2 == -1) {
			perror("write()");
			exit(EXIT_FAILURE);
		}

		if (close(fd) == -1) {
			perror("close");
			exit(EXIT_FAILURE);

		}

		exit(EXIT_SUCCESS);

	}

	else if (child_pid > 0){

		if (wait(NULL) == -1) {
					perror("wait error");
		}


		if (close(fd) == -1) {
			perror("close");
			exit(EXIT_FAILURE);
		}

		printf("bye!\n");

		exit(EXIT_SUCCESS);

	}

	else {
		perror("fork()");
		exit(EXIT_FAILURE);
	}
}

