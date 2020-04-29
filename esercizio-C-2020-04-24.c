/*
 * esercizio-C-2020-04-24.c
 *
 *  Created on: 24 apr 2020
 *      Author: utente
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <signal.h> // signal

#define SI 1
#define NO 0

int save_on_file = NO;

int return_save_status(){
	return save_on_file;
}


void child_writing(char* file_name, char* text, int text_len);


void child_process_signal_handler(int signum) {
	// NON è sicuro chiamare printf da un signal handler!
	// printf non è async-signal-safe (vedere Kerrisk sezione 21.1.2)
	// printf è usato qui solo a scopo diagnostico/didattico
	printf("[child] child_process_signal_handler\n");
	if (signum == SIGUSR1){
		save_on_file = SI;
		printf("[child] save_on_file setted on 1\n");
	}else {
		save_on_file = NO;
		printf("[child] save_on_file setted on 0\n");
	}

}

void parent_process_signal_handler(int signum) {
	// riceviamo SIGCHLD: Child stopped or terminated

	pid_t child_pid;

	// NON è sicuro chiamare printf da un signal handler!
	// printf non è async-signal-safe (vedere Kerrisk sezione 21.1.2)
	// printf è usato qui solo a scopo diagnostico/didattico
	printf("[parent] parent_process_signal_handler\n");

	child_pid = wait(NULL);

	printf("[parent] signal handler: processo %u è terminato, ora termino anche io\n", child_pid);

}


int main(int argc, char * argv[]) {

	char * file_name;
	char * text_to_write;
	int text_to_write_len;


	if (argc == 1) {
		printf("specificare come parametro il nome del file da creare e su cui scrivere\n");
		exit(EXIT_FAILURE);
	}

	file_name = argv[1];

	printf("scrivo nel file %s\n", file_name);

	// 1) il processo padre crea un file output.txt (se il file esiste già, va troncato a zero e sovrascritto).

	// 2) il processo padre tiene aperto il file output.txt, salvare il descrittore in una variabile fd
	int fd = open(file_name,
				  O_CREAT | O_TRUNC | O_WRONLY,
				  S_IRUSR | S_IWUSR // l'utente proprietario del file avrà i permessi di lettura e scrittura sul nuovo file
				 );

	if (fd == -1) { // errore!
		perror("open()");
		exit(EXIT_FAILURE);
	}

	// write something
	// https://loremipsum.io/generator/?n=5&t=p
	text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

	text_to_write_len = strlen(text_to_write) + 1; // consideriamo anche lo '\0' finale

	pid_t child_pid;
	// creo un processo figlio
	switch(child_pid = fork()){

		case 0 : // processo figlio
			child_writing(file_name,text_to_write, text_to_write_len);
			break;
		case -1:
			perror("fork()\n");
			exit(EXIT_FAILURE);
			break;
		default: // processo padre
			// SE ARRIVA UN SEGNALE DI TERMINAZIONE DAL PROCESSO FIGLIO, ERRORE!
			if (signal(SIGCHLD, parent_process_signal_handler) == SIG_ERR) {
				perror("signal");
				exit(EXIT_FAILURE);
			}
			printf("[Parent] pid=%u\n", getpid());
			break;

	// end switch
	}

	// Il processo padre inizia a scrivere
	// si potrebbe mettere in una funzione "parent_process()"
	int len_to_write = ceil(text_to_write_len/2);
	int res = write(fd, text_to_write, len_to_write);
	if (res == -1) {
		perror("write()");
		exit(EXIT_FAILURE);
	} else {
		printf("[Parent] Ho scritto %d bytes! write ok\n", res);
		off_t file_offset;
		file_offset = lseek(fd, 0, SEEK_CUR); // SEEK_CUR
		printf("lseek = %ld", file_offset);
		//close(fd);
	}

	printf("[Parent] Mando SIGUSR1 al processo figlio!\n");
	kill(child_pid, SIGUSR1);

	// attende un segnale dal figlio!
	pause(); // verremo svegliati da SIGCHLD, e quindi si invoca il signal handler
	// viene invocato
	// parent_process_signal_handler(SIGCHLD) che proseguirà l'esecuzione!!
	close(fd);

	exit(EXIT_SUCCESS);
}

void child_writing(char* file_name, char* text, int text_len){
	// processo figlio che scrive la seconda metà del testo

	printf("Entrato in child_writing!\n");
	printf("[child] pid=%u\n", getpid());

	if(signal(SIGUSR1, child_process_signal_handler) == SIG_ERR) {
			perror("signal");
			exit(EXIT_FAILURE);
	}

	if (signal(SIGTERM, child_process_signal_handler) == SIG_ERR) {
			perror("signal");
			exit(EXIT_FAILURE);
	}
	// ciclo infinito
	while(1){

		if(return_save_status() == SI){

			int fd = open(file_name,
						  O_CREAT | O_WRONLY,
						  S_IRUSR | S_IWUSR // l'utente proprietario del file avrà i permessi di lettura e scrittura sul nuovo file
						 );
			if (fd == -1) { // errore!
				perror("open()");
				exit(EXIT_FAILURE);
			}

			// raggiungo la poszione con "lseek()"
			off_t file_offset;
			file_offset = lseek(fd, 0, SEEK_END); // SEEK_CUR: posizione relativa rispetto alla posizione corrente

			if (file_offset == -1) {
				perror("lseek()");
				exit(EXIT_FAILURE);
			}

			printf("current file offset: %ld\n", file_offset);

			int len_to_write = text_len - ceil(text_len/2) - 1;
			text = text + file_offset;
			int res = write(fd, text, len_to_write);
			if (res == -1) {
				perror("write()");
				exit(EXIT_FAILURE);
			} else {
				printf("[Child] Ho scritto %d bytes! write ok\n", res);
				close(fd);
				// terminazioen del processo
				exit(EXIT_SUCCESS);
			}

		} else {
			pause(); // va in pausa e viene svegliato dal segnale del genitore!
		}

}
