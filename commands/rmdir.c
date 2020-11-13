#include <stdio.h>
#include <unistd.h>

int main(int argc, char ** argv){
	if (argc <= 1){
		fprintf(stderr, "minsh: Too few arguments\n");
		return 1;
	}
	if (argc > 1){
		fprintf(stderr, "minsh: Too many arguments\n");
		return 1;
	}
	int i = 1;
	for (i = 1 ; i < argc ; i++){
		if ( rmdir(argv[i]) < 0 ){
			fprintf(stderr, "minsh: %s - ", argv[i]);
			perror("");
			continue;
		}
	}
	return 0;
}
