#ifndef _semaphore
#define _semaphore

union semun
{
    int val; 
    struct semid_ds *buf;
    unsigned short *array; 
};

int CreateSem(key_t key,int value);
int Sem_P(int semid);
int Sem_V(int semid);
int GetvalueSem(int semid);
void DestroySem(int semid);


#endif
