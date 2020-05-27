/*
 * esercizio-C-2020-04-24.c
 *
 *  Created on: 24 apr 2020
 *      Author: utente
 */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>



int main(int argc, char * argv[]) {

	char * file_name;
	file_name="output.txt";
	int res;
	off_t file_offset;

	char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";
	int text_to_write_len=strlen(text_to_write);

	int fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

	if (fd == -1) {
		perror("open()");
		exit(EXIT_FAILURE);
	};

	res = ftruncate(fd, text_to_write_len*sizeof(char));

	if (res == -1) {
		perror("ftruncate()");
		exit(EXIT_FAILURE);
	}

	int dispari=text_to_write_len%2;

	switch (fork()) {
		case -1:
			perror("problema con fork");

			exit(EXIT_FAILURE);

		case 0:

			printf("CHILD: scrivo %d caratteri nella seconda metà del file, a partire dall'offset %d\n",text_to_write_len/2+dispari,text_to_write_len/2);

			file_offset=lseek(fd,text_to_write_len/2,SEEK_SET); //mi sposto nella posizione corretta

			if (file_offset == -1) {
					perror("lseek()");
					exit(EXIT_FAILURE);
				}
			res=write(fd,&text_to_write[text_to_write_len/2],text_to_write_len/2+dispari);
			//NOTA: text_to_write_len/2+dispari
			//il +dispari tiene conto del fatto che il numero di caratteri del testo da scrivere
			//potrebbe essere dispari

			if (res == -1) {
				perror("write()");
				exit(EXIT_FAILURE);
			};

			printf("CHILD: finito, ciao!\n");

			exit(EXIT_SUCCESS);

		default:
			//sleep(1);
			//lseek(fd,0,SEEK_SET);
			//se decommento il figlio scrive per primo quindi devo riportare il file_offset a 0

			printf("PARENT: scrivo nella prima metà del file %d caratteri\n",text_to_write_len/2);

			res=write(fd,text_to_write,text_to_write_len/2);

			if (res == -1) {
				perror("write()");
				exit(EXIT_FAILURE);
			};

			printf("PARENT: ho scritto fino alla posizione offset=%d, bye!\n",text_to_write_len/2-1);
	}

	exit(EXIT_SUCCESS);
}




