#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv){
	if (argc < 3){
		fprintf(stderr, "minsh: Not enough arguments\n");
		return 1;
	}
	struct stat dirstat;
	if (stat(argv[argc-1], &dirstat) < 0 || ! (S_ISDIR(dirstat.st_mode))){	

		if ( argc > 3 ){
			fprintf(stderr, "minsh: Too many arguments\n");
			return 1;
		}
		
		int fd_src, fd_dest;
		char buffer[1024];
		int nbytes;

		fd_src = open(argv[1], O_RDONLY);

		if ( fd_src < 0 ){
			perror("minsh");
			return 1;
		}

		fd_dest = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0755);
		
		if ( fd_dest < 0 ){
			perror("minsh");
			return 1;
		}

		nbytes = read(fd_src, buffer, 1024);
		while (1){
			if ( nbytes < 0 ){
				perror("minsh");
				close(fd_src);
				close(fd_dest);
				return 1;
			}
			else if ( nbytes == 0 ){
				break;
			}
			else{
				write(fd_dest, buffer, nbytes);
			}
			nbytes = read(fd_src, buffer, 1024);
		}
		close(fd_src);
		close(fd_dest);
		

		if ( unlink(argv[1]) < 0 ){
			perror("minsh");
			return 1; 
		}

		return 0;
	}

	int i = 1;
	int fd_src, fd_dest;
	char buffer[1024];
	int nbytes;
	char dest[1024];

	for ( i = 1 ; i < argc - 1 ; i++ ){
		fd_src = open(argv[i], O_RDONLY);

		if (fd_src < 0){
			perror("minsh");
			return 1;
		}

		strcpy(dest, argv[argc-1]);
		strcat(dest, "/");
		strcat(dest, argv[i]);

		fd_dest = open(dest, O_CREAT | O_WRONLY | O_TRUNC, 0755);
		
		if ( fd_dest < 0 ){
			perror("minsh");
			return 1;
		}

		nbytes = read(fd_src, buffer, 1024);
		while (1){
			if ( nbytes < 0 ){
				perror("minsh");
				close(fd_src);
				close(fd_dest);
				return 1;
			}
			else if ( nbytes == 0 ){
				break;
			}
			else{
				write(fd_dest, buffer, nbytes);
			}
			nbytes = read(fd_src, buffer, 1024);
		}
		close(fd_src);
		close(fd_dest);

		if ( unlink(argv[i]) < 0 ){
			perror("minsh");
			return 1; 
		}

	}
	return 0;
}

