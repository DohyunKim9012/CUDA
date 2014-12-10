#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
		// arguments
		int db_count = 0;
		int i = 1;
		char *db_file = NULL;
		int dsz = 0;
    int sz = 0;

		// internal variables
		char *db_data;
		int db_fd;

		// argument parsing
		while(i < argc){
				if(strcasecmp(argv[i], "-h") == 0){
						printf("\nUsage: %s -o <output_file_name> -rtn <record_num> -dsz <data_size>\n", argv[0]);
						printf("\t-rtn <record_num> : total number of db records\n\n");
						exit(1);
				}
				else if(strcasecmp(argv[i], "-o") == 0){
						if(i != argc-1)	db_file = argv[++i];
				}
				else if(strcasecmp(argv[i], "-rtn") == 0){
						if(i != argc-1) db_count = atoi(argv[++i]);
				}
				else if(strcasecmp(argv[i], "-dsz") == 0){
						if(i != argc-1) dsz = atoi(argv[++i]);
				}
				
				i++;
		}

		// argument error checking
		if(db_file == NULL){
				db_file = "simple.db";
		}
		
		if(db_count == 0){
				db_count = 10000;
		}
		
		if(dsz == 0){
				dsz = 1024;
		}


		// db generation
		db_fd = open(db_file, O_CREAT | O_EXCL | O_RDWR, 0644);

		if(db_fd == -1){
			fprintf(stderr, "File open error\n");
			exit(1);
		}

		db_data = malloc(sizeof(char) * dsz * 2);
		

		for(i = 0; i < db_count; i++){
				write(db_fd, &i, sizeof(int));
        write(db_fd, &sz, sizeof(int));
				write(db_fd, db_data, sizeof(char)*dsz*2);
		}
		
		free(db_data);
		
		printf("DB file creation is completed...\n");

		close(db_fd);
		return 0;
}
