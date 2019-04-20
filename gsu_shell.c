#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
/* Kendi baslik dosyalarimizi include edelim */
#include "parser.h"
#include "process.h"

#define VERSION "2.0"

/* Bekleme karakteri */
#define PROMPT  ">"

int main(int argc, char *argv[]) {

	/* Cocuk surecin veya bir icsel komutun basari durumunu tutan
	 * degisken. */
	int child_retval = 0;
int h=0,k=0,i=0;
	/* TODO: Kullanicinin ev dizinini cevresel degiskenlerden edinerek
	 * bir string'e kaydedin. */
	char *home_dir = getenv("HOME");
char hostname[50];
gethostname(hostname,50);
char history[10][10];
	/* Komut satiri ile ilgili bilgileri tutan struct'i tanimliyoruz. */
	CommandLine cl;

	/* Yukaridaki cl degiskenini gosteren bir gosterici tanimliyoruz. */
	CommandLine *cl_ptr = &cl;

	while (1) {
		/* TODO: Ekrana PROMPT ile tanimli komut satiri bekleyicisini yaziyoruz. */
		printf("%s%s  ", PROMPT,hostname);

		/* TODO: shell_process_line() ile satiri okuyup ayristiriyoruz. Artik cl_ptr
		 * ile gosterilen CommandLine yapisindaki komut satirina dair bilgiler
		 * var.
		 */

		shell_process_line(cl_ptr);
		


		/* TODO: Eger komut satiri okunurken bir hata olduysa, error_code degiskeni
		 * ayarlaniyor. shell_print_error() ise CommandLine struct'indaki error_code'un
		 * degerine bakarak hata varsa ekrana bunu basiyor. Hata olustuysa
		 * bunu shell_print_error() kullanarak ekrana yazdirin ve dongunun
		 * basina donun.
		 */
		if (cl_ptr->error_code) {
			shell_print_error(cl_ptr);
			shell_free_args(cl_ptr);
		}

		/* Boru hatti olsun olmasin, artik cl_ptr->first_argv icinde bir komut
		 * yaziyor. Eger NULL ise kullanici hicbir sey yazmadan Enter'a basip
		 * gecti demektir.
		 */
		if (cl_ptr->first_argv != NULL) {
			/* Once icsel komut mu degil mi diye bakiyoruz. Eger
			 * komut icsel ise, boru hatti olup olmamasiyla ilgilenmiyoruz,
			 * dogrudan yapilmasi gerekeni yapiyoruz.
			 * Burada bir if/else if/.. yapisiyla komutu taninan
			 * icsel komutlarla karsilastirip aksiyon almaniz gerekiyor.
			 */
if(cl_ptr->first_argv[0]){
					strcpy(history[h],cl_ptr->first_argv[0]);
}
if(k==10){
k=10;}

if(h==10){
h=0;}
			



			/* TODO: version */
			if (strcmp(cl_ptr->first_argv[0], "version") == 0) {
				printf("v %s\n", VERSION);
				child_retval = 0;
			}

			/* TODO: exit */
			else if (strcmp(cl_ptr->first_argv[0], "exit") == 0) {
				printf("Program terminated \n");
				child_retval = 0;
				shell_free_args(cl_ptr);
				exit(0);
			}
			
			
			else if (strcmp(cl_ptr->first_argv[0], "history") == 0) {
			int j=k;
				for( i=0;i<j;i++){
				printf("%s \n",history[i]);
				}
				child_retval = 0;
				shell_free_args(cl_ptr);
			continue;	
			}
			

			/* TODO: cd */
			else if (strcmp(cl_ptr->first_argv[0], "cd") == 0) {
				if (cl_ptr->first_argv[1] == NULL) {

					if (chdir(home_dir) < 0) {
						child_retval = 1;
						perror("gsu_shell: ");
						shell_free_args(cl_ptr);
					} else {
						child_retval = 0;
					}

				} else {

					if (chdir(cl_ptr->first_argv[1]) < 0) {
						child_retval = 1;
						perror("gsu_shell: ");
						shell_free_args(cl_ptr);
					} else {
						child_retval = 0;
					}

				}
			}
			

			/* TODO: !! */
			else if (strcmp(cl_ptr->first_argv[0], "!!") == 0) {
				printf("child return value : %d\n", child_retval);
				child_retval = 0;
			}
			
			
			/* TODO: Komut icsel degilse normal prosedur isleyecek:
			 * Komutlar cocuk surecler tarafindan exec edilecek.
			 * Boru hatti varsa ikinci komut icin de cocuk surec yaratilacak
			 * ve ilgili diger ayarlamalar yapilacak.
			 */
			else {
				/* Cocuk sureclerin PID'lerini tutmak icin. */
				pid_t first_child, second_child;
				
				int fd[2];
				int status = -1;
				
				if (cl_ptr->has_pipe == 1) {

					if (pipe(fd) < 0) {
						fprintf(stderr, "pipe error");
					}}
					if ((first_child = fork()) < 0) {
						perror("fork failure \n");
						shell_free_args(cl_ptr);
						exit(1);
					} else if (first_child == 0) {
						if (cl_ptr->has_pipe == 1) {
							close(STDOUT_FILENO);
							dup2(fd[1], 1);
							close(fd[0]);
						} 
						child_retval = shell_exec_cmd(cl_ptr->first_argv);
						shell_free_args(cl_ptr);
						exit(child_retval);
					}
					if (cl_ptr->has_pipe == 1) {
						if ((second_child = fork()) < 0) {
							perror("fork failure \n");
							shell_free_args(cl_ptr);
							exit(EXIT_FAILURE);
						} else if (second_child == 0) {
							close(STDIN_FILENO);
							dup2(fd[0], 0);
							close(fd[1]);
					
							child_retval = shell_exec_cmd(cl_ptr->second_argv);
							shell_free_args(cl_ptr);
							exit(child_retval);
						}
					}
					/* Ebeveyn cocugu/cocuklari yarattiktan sonra buradan
					 * devam ediyor */
					close(fd[0]);
					close(fd[1]);
					waitpid(first_child, &status, 0);

					if (cl_ptr->has_pipe == 1) {
						waitpid(second_child, &status, 0);
					}

					if (WIFEXITED(status)) {
						child_retval = WEXITSTATUS(status);
					}
					

					shell_free_args(cl_ptr);
				} /* else */
			} /* if (..) */
			printf(" \n");
h++;
k++;

					
		} /* while(1) */

		return 0;
	}

