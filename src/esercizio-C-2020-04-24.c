/*
 * esercizio-C-2020-04-24.c
 *
 *  Created on: 24 apr 2020
 *      Author: utente
 */
/***************TESTO ESERCIZIO***************
scrivere un programma in C:

è data la stringa di caratteri:

char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

il processo padre crea un file output.txt (se il file esiste già, va troncato a zero e sovrascritto).

il processo padre tiene aperto il file output.txt, salvare il descrittore in una variabile fd

il processo padre crea un processo figlio: l'obiettivo finale è che il file output.txt contenga tutta la stringa text_to_write e che i due processi (processo padre e processo figlio) si dividano il lavoro di scrittura nel file. La scrittura sul file avviene in "parallelo".

il processo padre scrive la prima metà di text_to_write nel file, cominciando a scrivere dalla posizione iniziale del file.

il processo figlio scrive l'altra metà di text_to_write del file, cominciando a scrivere da dove il processo padre ha terminato di scrivere.

il processo padre attende la conclusione del processo figlio e poi termina.
*/
/***************TESTO ESERCIZIO***************
scrivere un programma in C:

è data la stringa di caratteri:

char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

il processo padre crea un file output.txt (se il file esiste già, va troncato a zero e sovrascritto).

il processo padre tiene aperto il file output.txt, salvare il descrittore in una variabile fd

il processo padre crea un processo figlio: l'obiettivo finale è che il file output.txt contenga tutta la stringa text_to_write e che i due processi (processo padre e processo figlio) si dividano il lavoro di scrittura nel file. La scrittura sul file avviene in "parallelo".

il processo figlio apre per conto suo il file output.txt.

il processo padre scrive la prima metà di text_to_write nel file, cominciando a scrivere dalla posizione iniziale del file.

il processo figlio scrive l'altra metà di text_to_write del file, cominciando a scrivere da dove il processo padre ha terminato di scrivere.

il processo padre attende la conclusione del processo figlio e poi termina.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>

char* substring(char* text, int start, int end);	//funzione che ritorna una sottostringa di text (dalla posizione start a quella end-1 incluse)

int main(int argc, char * argv[]) {
	char * file_name = "output.txt";
	char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";
	int text_to_write_len;
	off_t file_offset;
	int res, fd;

	// creo/tronco il file
	fd = open(file_name,
				  O_CREAT | O_TRUNC,
				  S_IRUSR | S_IWUSR
				 );
	if (fd == -1) { // errore!
		perror("open()");
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	text_to_write_len = strlen(text_to_write) + 1; // consideriamo anche lo '\0' finale

	switch (fork()) {
		case -1:
			perror("problema con fork");

			exit(EXIT_FAILURE);

		case 0: // processo FIGLIO
			fd = open(file_name,
						  O_WRONLY,
						  S_IRUSR | S_IWUSR
						 );

			if (fd == -1) { // errore!

				perror("open()");

				exit(EXIT_FAILURE);
			}

			file_offset = lseek(fd, text_to_write_len / 2, SEEK_SET); // SEEK_SET: posizione rispetto all'inizio del file

			if (file_offset == -1) {
				perror("lseek()");
				exit(EXIT_FAILURE);
			}

			res = write(fd, substring(text_to_write, text_to_write_len/2, text_to_write_len), text_to_write_len - text_to_write_len / 2);
			// write restituisce il numero di bytes che ha scritto
			// in caso di errore, restuisce -1

			if (res == -1) {
				perror("write()");
				exit(EXIT_FAILURE);
			}

			if (close(fd) == -1) {
				perror("close");
				exit(EXIT_FAILURE);
			}

			exit(0);
			break;

		default:
			// processo PADRE
			fd = open(file_name,
						  O_WRONLY,
						  S_IRUSR | S_IWUSR
						 );

			if (fd == -1) { // errore!

				perror("open()");

				exit(EXIT_FAILURE);
			}
			file_offset = lseek(fd, 0, SEEK_SET); // SEEK_SET: posizione rispetto all'inizio del file

			if (file_offset == -1) {
				perror("lseek()");
				exit(EXIT_FAILURE);
			}

			res = write(fd, text_to_write, text_to_write_len / 2);
			// write restituisce il numero di bytes che ha scritto
			// in caso di errore, restuisce -1

			if (res == -1) {
				perror("write()");
				exit(EXIT_FAILURE);
			}

			if (close(fd) == -1) {
				perror("close");
				exit(EXIT_FAILURE);
			}

			if (wait(NULL) == -1) {
				perror("wait error");
			}
	}


	printf("bye!\n");
	return 0;
}

char* substring(char* text, int start, int end){	//include text[start] ed esclude text[end]
	if(start < 0 || end > strlen(text)+1 || start > end){
		perror("indici sbagliati\n");
		exit(EXIT_FAILURE);
	}

	char* res = malloc(end - start);

	int i = 0;
	while(start < end){
		res[i] = text[start];
		start++;
		i++;
	}

	return res;
}
