#include "master.h"
#define IP_FILE_PATH "./IPs"

//Iterate over IPs file and get the topology
int main() {

	struct Head_Node *topology = NULL;
	struct Node *node = NULL;
	FILE *fp = NULL;
	struct stat file_stat;
	int version = 0;
	time_t last_mtime = 0;
	
	while(1) {
		
		if(stat(IP_FILE_PATH, &file_stat) != -1) {
			if(last_mtime <= file_stat.st_mtime) {
				fp = fopen(IP_FILE_PATH, "r");
				(void)parse_IPs(fp, &topology, &version);
				fclose(fp);
			}
		}
		
		sleep(1);
	}
		
	return 0;
}
