#include <stdio.h>
#import "host.h"
#include <pthread.h>
int main() {

    pthread_t  thread_id;
    pthread_create(&thread_id, NULL, &open_console, NULL);
    pthread_t  thread_id2;
    pthread_create(&thread_id2, NULL, &config, NULL);
    pthread_t  thread_id3;
    pthread_create(&thread_id3, NULL, &escuchar_Host, NULL);
    pthread_join(thread_id, NULL);
    pthread_join(thread_id2, NULL);
    pthread_join(thread_id3, NULL);


    return 0;
}