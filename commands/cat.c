#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char ** argv){
	int i = 1;
	int fd;
	if ( (fd = open(argv[i], O_RDONLY)) < 0 ){
		//读取文件
		fprintf(stderr, "\nminsh: %s - ", argv[i]);
		perror("");
		return 0;
	}

	printf("\n\n%s:\n\n", argv[i]);

	char buffer[1024];
	int nbytes;	
	while ( 1 ){
		// 从文件读取数据
		nbytes = read(fd, buffer, 1024);
		if (nbytes < 0){
			perror("\nminsh");
			break;
		}
		else if (nbytes == 0){// 文件结束
			write(1, "\n", 1);
			break;
		}
			
		// Write to output
		if ( write(1, buffer, nbytes) < 0 ){	
			perror("\nminsh");
			break;
		}
	}

	close(fd);
	printf("\n\n");
	return 0;
}

