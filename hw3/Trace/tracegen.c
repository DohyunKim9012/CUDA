#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#define MODE_SEQUENTIAL     0
#define MODE_RANDOM         1

int main(int argc, char* argv[])
{
		// arguments
		int db_count = 0;
		int rec_count = 0;
		int req_count = 0;
		int w_fd = -1;
		int i = 1;
		int mode = -1;

		// internal variables
		int g_seq = 0;
		int g_size = 0;

		struct timeval tv;
		char buf[100];
		
		while(i < argc){
				if(strcasecmp(argv[i], "-h") == 0){
						printf("\nUsage: %s -o <outputfile> [-s|-r] -rtn <record_num> -dsz <data_size> -rn <req_num> -f\n", argv[0]);
						printf("\t-s : sequential access to db\n\t-r : random access to db\n");
						printf("\t-rtn <record_num> : total number of db records\n");
						printf("\t-dsz <data_size> : size of record\n");
						printf("\t-rn <req_num> : number of requesets\n");
						exit(1);
				}
				else if(strcasecmp(argv[i], "-o") == 0){
						if(i != argc-1)	w_fd = open(argv[++i], O_CREAT | O_RDWR, 0644);
				}
				else if(strcasecmp(argv[i], "-s") == 0)
						mode = MODE_SEQUENTIAL;
				else if(strcasecmp(argv[i], "-r") == 0)
						mode = MODE_RANDOM;
				else if(strcasecmp(argv[i], "-rtn") == 0){
						if(i != argc-1) db_count = atoi(argv[++i]);
				}
				else if(strcasecmp(argv[i], "-dsz") == 0){
						if(i != argc-1) rec_count = atoi(argv[++i]);
				}
				else if(strcasecmp(argv[i], "-rn") == 0){
						if(i != argc-1) req_count = atoi(argv[++i]);
				}

				i++;
		}

		// argument error checking
		if(w_fd == -1){
				w_fd = open("trace.txt", O_CREAT | O_RDWR, 0644);
		}
	   
		if(mode == -1){
				mode = MODE_SEQUENTIAL;
		}
		
		if(db_count == 0){
				db_count = 10000;
		}
		
		if(rec_count == 0){
				rec_count = 1024;
		}
		
		if(req_count == 0){
				req_count = 200;
		}


		// trace generation
		gettimeofday(&tv, NULL);
		srand(tv.tv_usec);
		
		switch(mode){
			case MODE_SEQUENTIAL:

				if(req_count > db_count) req_count = db_count;

				for(i = 0; i < req_count; i++){
						g_seq = i;

						g_size = rec_count;

						sprintf(buf, "%d\t%d\n", g_seq, g_size);
						write(w_fd, buf, strlen(buf));
				}
				
				break;
			
			case MODE_RANDOM:

				for(i = 0; i < req_count; i++){
						g_size = rec_count;
						
						sprintf(buf, "%d\t%d\n", (rand() % db_count) +1, g_size);
						write(w_fd, buf, strlen(buf));
				}

				break;
		}

		printf("Trace file creation is completed...\n");
		
		close(w_fd);
		return 0;
}
