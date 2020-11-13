#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv){
	//检测是否含有三个参数
	if (argc < 3){
		fprintf(stderr, "minsh: Not enough arguments\n");
		return 1;
	}
	struct stat dirstat;
	//获取该文件的属性	
	if (stat(argv[argc-1], &dirstat) < 0 || ! (S_ISDIR(dirstat.st_mode))){
		//如果指定的目录不存在，它必须是一个文件
		//或者，如果文件已经存在，则覆盖

		if ( argc > 3 ){
			fprintf(stderr, "minsh: Too many arguments\n");
			return 1;
		}
		int fd,fd1;
		char buffer[1024];
		int nb;
		fd = open(argv[1], O_RDONLY);
		if ( fd < 0 ){
			perror("minsh");
			return 1;
		}
		fd1 = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0755);
		
		if (fd1 < 0 ){
			perror("minsh");
			return 1;
		}

		nb = read(fd, buffer, 1024);
		while (1){
			if ( nb < 0 ){	// 出现错误
				perror("minsh");
				close(fd);
				close(fd1);
				return 1;
			}
			else if ( nb == 0 ){	
				break;
			}
			else{
				write(fd1, buffer, nb);
			}
			nb = read(fd, buffer, 1024);
		}
		close(fd);
		close(fd1);
		
		// // 删除源文件
		// if ( unlink(argv[1]) < 0 ){
		// 	perror("minsh");
		// 	return 1; 
		// }

	}
	return 0;
}

