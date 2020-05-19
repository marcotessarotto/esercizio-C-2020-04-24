#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <sys/wait.h>



int main(int argc, char *argv[]) {

	char * file_name;
	char * text_to_write;
	int text_to_write_len;


	file_name = "output.txt";
	printf("scrivo nel file %s\n", file_name);

	int fd = open(file_name,  O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR );

	if (fd == -1) { // errore!

		perror("open()");
		exit(EXIT_FAILURE);
	}

	text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

	text_to_write_len = strlen(text_to_write) + 1;

	int blocco1 = text_to_write_len/2;

	switch(fork()){


	case 0:
		printf("[child] scrivo %d bytes\n", text_to_write_len-blocco1);
		int res_c = write(fd, &text_to_write[blocco1], text_to_write_len-blocco1);

		if (res_c == -1) {
			perror("write()");
			exit(EXIT_FAILURE);
		}
		exit(0);
		break;

	case -1:
		printf("fork() ha fallito! niente processo figlio!\n");

		exit(1);
		break;

	default:
		printf("[parent] scrivo %d bytes\n", blocco1);

		int res_p = write(fd, text_to_write, blocco1);

		if (res_p == -1) {
			perror("write()");
			exit(EXIT_FAILURE);
		}

		if (wait(NULL) == -1) {
			perror("wait error");
		} else {
			printf("[parent] il processo figlio ha terminato! ora termina il process padre.\n");
		}
		exit(0);
		break;

	}

	// close file

	if (close(fd) == -1) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	printf("bye!\n");

	exit(EXIT_SUCCESS);
}
