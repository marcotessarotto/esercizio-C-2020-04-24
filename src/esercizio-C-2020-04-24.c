/*
 * esercizio-C-2020-04-24.c
 *
 *  Created on: 24 apr 2020
 *      Author: Leonardo Simonini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ctype.h>

int findDimension(char * array);

char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

int main(int argc, char *argv[]){
	int dimension = findDimension(text_to_write);
	int length = dimension;
	int middle_position = length/2;
	int res;
	off_t offset;
	int fd = open("output.txt",
					  O_CREAT | O_TRUNC | O_WRONLY,
					  S_IRUSR | S_IWUSR // l'utente proprietario del file avrà i permessi di lettura e scrittura sul nuovo file
					 );
	if (fd == -1) {
			perror("open error\n");
			return 0;
		}

	switch(fork()){
		case 0:
				offset = lseek(fd, middle_position, SEEK_SET);
				if (offset == -1) {
					perror("error in lseek\n");
					exit(1);
				}
				int string_length = length - middle_position;
				res = write(fd, &text_to_write[middle_position], string_length);
				if(res == -1){
					perror("error in write fo child");
					exit(1);
				}
				exit(EXIT_SUCCESS);
				close(fd);
				break;
		case -1: perror("error in fork");
				 exit(1);
				 break;

		default: offset = lseek(fd, 0, SEEK_SET);
				 if (offset == -1) {
					 	perror("error in lseek\n");
					 	exit(1);
				 }
				 res = write(fd, text_to_write, middle_position);

				 if(res == -1){
					perror("error in write fo child");
					exit(1);
	 			}
				 if(wait(NULL) == -1){
					 perror("Error in wait");
					 exit(EXIT_FAILURE);
				 }
				 close(fd);
				 printf("Chiudo.\n");
	}
}

int findDimension(char * array){
	int count = 0;
	while(array[count] != '\0'){
		count++;
	}

	return count;

}
