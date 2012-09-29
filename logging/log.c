#include "log.h"
 
FILE *log_fp;
char buf[500];
char buf1[200];

time_t timer;
char buffer[25];
struct tm* tm_info;
level log_level = DEBUG;
pthread_mutex_t log_mutex;

void log_init()
{
   sprintf(buf, "/tmp/machine.%d.log", MACHINE_ID);
   log_fp = fopen(buf, "a");
   pthread_mutex_init(&log_mutex, NULL);
}

/*void main()
{
    log_init();
    LOG(DEBUG, "Hello %s", "world");
    log_exit();
}*/

void log_exit()
{
    fclose(log_fp);
}
