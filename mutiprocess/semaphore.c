#include <stdlib.h>
#include <fcntl.h>
#include <sys/sem.h>
#include "semaphore.h"

int CreateSem(key_t key,int value)
{
	union semun sem;
	int semid;
	sem.val=value;
	semid=semget(key,1,IPC_CREAT);
	if (semid==-1){
			perror("semget error");	exit(1);
	}
	semctl(semid,0,SETVAL,sem);
	return semid;
}

int Sem_P(int semid)
{
	struct sembuf sops={0,-1,IPC_NOWAIT};
	return (semop(semid,&sops,1));
}

int Sem_V(int semid)
{
	struct sembuf sops={0,+1,IPC_NOWAIT};
	return (semop(semid,&sops,1));
}

int GetvalueSem(int semid)
{
	union semun sem;
	return semctl(semid,0,GETVAL,sem);
}
void DestroySem(int semid)
{
	union semun sem;
	sem.val=0;

	semctl(semid,0,IPC_RMID,sem);
}
