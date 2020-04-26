#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){

	char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";
	int text_to_write_len = strlen(text_to_write);

	// open a file for writing
	int fd = open("output.txt",
				  O_CREAT | O_TRUNC | O_WRONLY,
				  S_IRUSR | S_IWUSR // l'utente proprietario del file avrà i permessi di lettura e scrittura sul nuovo file
				 );
	if (fd == -1) { // errore!
		perror("open()");
		exit(EXIT_FAILURE);
	}

	// usiamo ftruncate per impostare la dimensione del file (appena creato) a text_to_write_len byte
	int res = ftruncate(fd, text_to_write_len);
	if( res == -1){
		perror("ftruncate()");
		exit(EXIT_FAILURE);
	}

	int father_string_part_length = (text_to_write_len/2);
	int child_string_part_length = text_to_write_len - (text_to_write_len/2);

	pid_t child_pid = fork();
	switch (child_pid) {
		case 0:
		{
			//child
			// SEEK_SET: posizione offset a (text_to_write_len/2) rispetto all'inizio del file
			off_t file_offset = lseek(fd, text_to_write_len/2 , SEEK_SET);
			if(file_offset == -1){
				perror("lseek()");
				exit(EXIT_FAILURE);
			}

			int res = write(fd, text_to_write + father_string_part_length, child_string_part_length);
			if(res == -1){
				perror("write()");
				exit(EXIT_FAILURE);
			}

			// close file
			if (close(fd) == -1) {
				perror("close");
				exit(EXIT_FAILURE);
			}

			exit(0);
			break;
		}
		case -1:
			printf("fork() ha fallito! niente processo figlio!\n");
			exit(1);
			break;
		default:
		{
			// father

			off_t file_offset = lseek(fd, 0, SEEK_SET);
			if(file_offset == -1){
				perror("lseek()");
				exit(EXIT_FAILURE);
			}

			int res = write(fd, text_to_write, father_string_part_length);
			if(res == -1){
				perror("write()");
				exit(EXIT_FAILURE);
			}

			if(wait(NULL) == -1){
				perror("wait()");
				exit(EXIT_FAILURE);
			}
		}
	}

	// close file
	if (close(fd) == -1) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	exit(0);
}
