# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
# include <errno.h>
# include <limits.h>

#include <time.h>

#include <sys/mman.h>   /* shared memory and mmap() */
#include <pthread.h> /* POSIX Threads */

// ref: http://man7.org/training/download/posix_shm_slides.pdf
// ref : http://stackoverflow.com/questions/2584678/how-do-i-synchronize-access-to-shared-memory-in-lynxos-posix?rq=1

int main(int argc, char *argv[])
{
	
	if(argc!=2)
	{
		printf("Usage: %s <MSG_SIZE>\n", argv[0]);
		exit(0);
	}
	
	/*// GETTIME STUFFF 
	clockid_t clk_id;
	struct timespec tp_start, tp_end;
	int time_elapsed_sec;
	long long time_elapsed_nsec;
	
	clk_id = CLOCK_MONOTONIC;
	long BILLION = 1000000000L;
	// END GETTIME STUFF*/
	
	
	int retVal ;
	
	int fd;
	size_t MSG_SIZE;
	void *send_buf_temp;

	char name[] = "/my_shmsname2";

	MSG_SIZE=atoi(argv[1]);
	printf("\t");
	//printf("Size : %ld\n", MSG_SIZE);
	// Creating shared buff
	fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	//fd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0777);
	//printf("SHM_OPEN : %d\n", fd);
	//printf("  ");
	retVal = ftruncate(fd, MSG_SIZE+1);
	
	
	
	send_buf_temp = mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	char * send_buf = send_buf_temp;
	char recv_buf[MSG_SIZE+1];
	
	close(fd);
	if(send_buf == MAP_FAILED)
		{
			perror("Mmap failed my_shmsname..");
			return 0;
		}
	//printf("Map passed\n");

	// Putting sync primitives in shared space	
	typedef struct
	{
		pthread_mutex_t ipc_mutex;
		pthread_cond_t  ipc_condvar;		
		pthread_cond_t  ipc_startvar;	
	} shared_data_t;

	fd = shm_open("/my_syncname2", O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
	//printf("SHM_OPEN sync : %d\n", fd);
	ftruncate(fd, sizeof(shared_data_t));
	shared_data_t* sdata = (shared_data_t*)mmap(0, sizeof(shared_data_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if(send_buf == MAP_FAILED)
		{
			perror("Mmap failed my_syncname..");
			return 0;
		}
	close(fd);
	
	
	// End putting in shared space
	
	
	if ( fork() != 0 )
	{ 
		/* Parent */
		
		pthread_condattr_t cond_attr;
		pthread_condattr_init(&cond_attr);
		pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
		pthread_cond_init(&sdata->ipc_condvar, &cond_attr);
				
		pthread_condattr_t cond_attr2;
		pthread_condattr_init(&cond_attr2);
		pthread_condattr_setpshared(&cond_attr2, PTHREAD_PROCESS_SHARED);
		pthread_cond_init(&sdata->ipc_startvar, &cond_attr2);
		
		pthread_mutexattr_t mutex_attr;
		pthread_mutexattr_init(&mutex_attr);
		pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&sdata->ipc_mutex, &mutex_attr);
		
		int j=0;
		for(j=0;j<100;j++)
		{
			// START TIME
			//clock_gettime(clk_id, &tp_start);
			
			//printf("P : initialization done\n");
			pthread_mutex_lock(&sdata->ipc_mutex);
			pthread_cond_wait(&sdata->ipc_startvar, &sdata->ipc_mutex);
			//printf("P : GOT THE LOCK.. going to write\n");
			
			
			// write stuff
			int i;
			for(i = 0; i<MSG_SIZE;i++)
				send_buf[i] = 'A';
			send_buf[MSG_SIZE] = '\0';
			
			//printf("P: Done with writing...\n");
			pthread_cond_signal(&sdata->ipc_condvar);
			//printf("P : Done with signalling...\n");
			pthread_mutex_unlock(&sdata->ipc_mutex);
			
			// read stuff
			pthread_mutex_lock(&sdata->ipc_mutex);
			pthread_cond_wait(&sdata->ipc_startvar, &sdata->ipc_mutex);
			for(i=0;i<MSG_SIZE;i++)
				recv_buf[i] = send_buf[i];
			recv_buf[MSG_SIZE] = '\0';
			
			pthread_mutex_unlock(&sdata->ipc_mutex);
			//clock_gettime(clk_id, &tp_end);	
			
			// END TIME
			
			/*
			printf("P : Done with read..\n");
			// printf("P : reads..%s\n", recv_buf);
			printf("P : reads..%ld\n", strlen(recv_buf));
			*/
			/*
			time_elapsed_sec = (tp_end.tv_sec - tp_start.tv_sec);
			time_elapsed_nsec = (tp_end.tv_nsec - tp_start.tv_nsec);
			//printf("%ld\t%lld\n", MSG_SIZE, ((BILLION*time_elapsed_sec)+time_elapsed_nsec)/2);
			printf("%lld\n", ((BILLION*time_elapsed_sec)+time_elapsed_nsec)/2);
			*/
			printf("%d\n", j);
		}
		
		//printf("P : so long\n");
	
		pthread_mutex_destroy(&sdata->ipc_mutex);
		pthread_cond_destroy(&sdata->ipc_startvar);
		pthread_cond_destroy(&sdata->ipc_condvar);
			
	}
	else
	{  
		int j=0;
		for(j=0;j<100;j++)
		{
			/* Child */
			//printf("IN THE CHILD\n");
			pthread_mutex_lock(&sdata->ipc_mutex);
			//printf("C : GOT THE LOCK.. going to wait\n");
			pthread_cond_signal(&sdata->ipc_startvar);
			pthread_cond_wait(&sdata->ipc_condvar, &sdata->ipc_mutex);
			// releases lock while waiting
			// read stuff
			// printf("C : Going to read..\n");
			int i;
			for(i=0;i<MSG_SIZE;i++)
				recv_buf[i] = send_buf[i];
			recv_buf[MSG_SIZE] = '\0';
			/*
			printf("C : reads..penultimate %d\n", recv_buf[MSG_SIZE-1] );
			printf("C : reads..last %d\n", recv_buf[MSG_SIZE] );
			printf("C : reads..%ld\n", strlen(recv_buf));
			printf("C : going to write.."); */
			// write stuff
			
			// START THRUPUT TIME
			for(i = 0; i<MSG_SIZE;i++)
				send_buf[i] = 'B';
			send_buf[MSG_SIZE] = '\0';
			// END THRUPUT TIME
			
			//printf("C : done with write, going to signal..\n");
			pthread_cond_signal(&sdata->ipc_startvar);
			pthread_mutex_unlock(&sdata->ipc_mutex);
			//printf("C : farewell\n");
		}
	}


	fflush(stdout);

	// This format should make for easy batch running..
	// printf("%d\t%ld", MSG_SIZE, total_time);

		//printf("Both : goign to unlink guy1\n");
		shm_unlink(name);
		//printf("Both : goign to unlink guy2\n");
		shm_unlink("/my_syncname2");
		
		//printf("Both : munmap sdata\n");
		if (munmap(sdata,sizeof(shared_data_t)))
			perror("munmap()");
			
		//printf("Both : munmap sendbuf\n");
		if (munmap(send_buf,MSG_SIZE+1))
			perror("munmap()");

	return 0;
}
