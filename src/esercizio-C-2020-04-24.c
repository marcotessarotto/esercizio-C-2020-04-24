/*
 * esercizio-C-2020-04-24.c
 *
 *  Created on: 24 apr 2020
 *      Author: utente
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

int main(int argc, char * argv[]) {

	char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

	int text_to_write_len_parent,text_to_write_len_child;

	text_to_write_len_parent = strlen(text_to_write)/2;
	text_to_write_len_child = strlen(text_to_write) - text_to_write_len_parent;

	int fd = open("output.txt",
				  O_CREAT | O_TRUNC | O_WRONLY,
				  S_IRUSR | S_IWUSR
				 );
	if (fd == -1) {

		perror("open()");

		exit(EXIT_FAILURE);
	}

	switch (fork()) {
				case 0:
					if (lseek(fd, text_to_write_len_parent , SEEK_SET) == -1) {
							perror("lseek()");
							exit(EXIT_FAILURE);
						}

					int reschild = write(fd, text_to_write + text_to_write_len_parent, text_to_write_len_child);

					if (reschild == -1) {
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

					if (write(fd, text_to_write, text_to_write_len_parent)== -1) {
						perror("write()");
						exit(EXIT_FAILURE);
					}

					if (wait(NULL) == -1) {
								perror("wait error");
							}

					exit(0);

			}

	if (close(fd) == -1) {
			perror("close");
			exit(EXIT_FAILURE);
		}

	printf("Fatto, bye!\n");


return 0;
}
