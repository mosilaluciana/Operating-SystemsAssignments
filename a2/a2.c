#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
sem_t sem1;
int ok=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void *thread_function(void *arg)
{
    int thread_id = *((int *)arg);
    pthread_mutex_lock(&mutex);

    if(thread_id == 4 && ok == 0)
    {
    
            pthread_cond_wait(&cond, &mutex);
            info(BEGIN, 6, thread_id);
        
        
    }
    else if(ok == 1 && thread_id == 4){
            info(BEGIN,6,thread_id);
        
    }

    else if(thread_id == 2)
    {
        ok = 1;
        info(BEGIN, 6, thread_id);
        pthread_cond_signal(&cond);
       
    }
    else
        info(BEGIN, 6, thread_id);
    
    if(thread_id ==2){
         pthread_cond_wait(&cond,&mutex);
        info(END, 6, thread_id);
    }
    else if(thread_id==4)
    { 
        info(END, 6, thread_id);
        pthread_cond_signal(&cond);
    }
    else 
       info(END, 6, thread_id);

    pthread_mutex_unlock(&mutex);

    return NULL;
}

void* thread_function2(void* arg)
{
    int thread_id = *((int*)arg);
    sem_wait(&sem1);
    info(BEGIN, 2, thread_id);
    info(END, 2, thread_id);
    sem_post(&sem1);
    return NULL;
}
void* thread_function3(void* arg)
{
    int thread_id = *((int*)arg);
    info(BEGIN, 3, thread_id);
    info(END, 3, thread_id);
    return NULL;
}
int main() {
    init();

    info(BEGIN, 1, 0);

    int pid2, pid3, pid4, pid5, pid6, pid7;
    int status;

    pid2 = fork();
    if (pid2 == 0) {
        sem_init(&sem1, 0,4);
        info(BEGIN, 2, 0);
 	pthread_t threads[36];
            int thread_args[36];

            for (int i = 0; i < 36; i++) {
			thread_args[i]=i+1;
                pthread_create(&threads[i], NULL, thread_function2, &thread_args[i]);
            }

            for (int i = 0; i < 36; i++) {
                pthread_join(threads[i], NULL);
            }
                   
           
        pid3 = fork();
        if (pid3 == 0) {
            info(BEGIN, 3, 0);
            pthread_t threads[4];
            int thread_args[4]={1,2,3,4};

            for (int i = 0; i < 4; i++) {
 
                pthread_create(&threads[i], NULL, thread_function3, &thread_args[i]);
            }

            for (int i = 0; i < 4; i++) {
                pthread_join(threads[i], NULL);
            }

            pid4 = fork();
            if (pid4 == 0) {
                info(BEGIN, 4, 0);

                pid5 = fork();
                if (pid5 == 0) {
                    info(BEGIN, 5, 0);
                    info(END, 5, 0);
                    _exit(0);
                }
                waitpid(pid5, &status, 0);
                info(END, 4, 0);
                _exit(0);
            }
            waitpid(pid4, &status, 0);
            info(END, 3, 0);
            _exit(0);
        }

        pid6 = fork();
        if (pid6 == 0) {
            info(BEGIN, 6, 0);
       pthread_t threads[5];
            int thread_args[5]={1,2,3,4,5};

            for (int i = 0; i < 5; i++) {
 
                pthread_create(&threads[i], NULL, thread_function, &thread_args[i]);
            }

            for (int i = 0; i < 5; i++) {
                pthread_join(threads[i], NULL);
            }
            info(END, 6, 0);
            _exit(0);
        }

        pid7 = fork();
        if (pid7 == 0) {
            info(BEGIN, 7, 0);
            info(END, 7, 0);
            _exit(0);
        }

        waitpid(pid3, &status, 0);
        waitpid(pid6, &status, 0);
        waitpid(pid7, &status, 0);
        sem_destroy(&sem1);
        info(END, 2, 0);
        _exit(0);
    }

    waitpid(pid2, &status, 0);
    info(END, 1, 0);
    return 0;
}
