#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdbool.h>
#include<time.h>

#define READ 0
#define WRITE 1

// Declaration of thread condition variable 
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER; 
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER; 

// declaring mutex 
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

//bool flags to tell monitor when to allow computaion
bool *shmPtr; 
int l1 = 1, l2=1,l3=1;

// void *monitor_c1(){
//     printf("inside monitor c1\n");
// 	pthread_mutex_lock(&mutex1);
// 	 //if(shmPtr[0] && !shmPtr[1]) then child 1 runs
//     while(!(shmPtr[0] && !shmPtr[1]))
//     {     
//         usleep(20);
//     }
//     printf("inside monitor c1 after while\n");
    
//     pthread_cond_signal(&cond1);//give timestamp also
//     printf("after cond_signal c1\n");
//     pthread_mutex_unlock(&mutex1);
//     printf("after mutex unlock c1\n");
	
// }

// void *monitor_c2(){
//     printf("inside monitor c2\n");
// 	pthread_mutex_lock(&mutex2);
// 		//if(!shmPtr[0] && shmPtr[1]) then child 2 runs
//     while(!(!shmPtr[0] && shmPtr[1]))
//     {  
//             usleep(20);
//     }
//     printf("inside monitor c2 after while\n");
//     pthread_cond_signal(&cond2); //give timestamp also
//     pthread_mutex_unlock(&mutex2);
 
// }

// void *monitor_c3(){
//     printf("inside monitor c3\n");
// 	pthread_mutex_lock(&mutex3);
// //if(!shmPtr[0] && !shmPtr[1]) then child 3 runs
//     while(!(!shmPtr[0] && !shmPtr[1]))         
//     {   
// 	    usleep(20);
  
//     }
//     printf("inside monitor c3 after while\n");
// 	pthread_cond_signal(&cond3);//give timestamp also
// 	pthread_mutex_unlock(&mutex3);

// }

void *add_nums(void *p){
    // printf("inside compute c1\n");
 	int *param = (int *)p;
 	pthread_mutex_lock(&mutex1);
    if(l1==1)
    {
        l1 = 2;
        pthread_cond_wait(&cond1, &mutex1);
        clock_t t;
        t = clock();
        printf("Child 1 running\n");
        long long sum = 0;
        
        for(int i=1; i<= *param; i++)
        {
        	sum += i;
        }
        t = clock() - t;
        double tt = ((double)t);
        printf("Child 1 completed after %lf microseconds\n", tt);
        return (void *)sum;
    }
    else
    {
        //Child 1 montior thread
        while(!(shmPtr[0] && !shmPtr[1]))
        {     
            usleep(20);
        }
        pthread_cond_signal(&cond1);
        printf("monitor thread 1 completed\n");
    }

	pthread_mutex_unlock(&mutex1);

    // printf("child 1 completed\n");
}

void *read_nums(void *p){
    // printf("inside compute c2\n");
 	int *param = (int *)p;
 	pthread_mutex_lock(&mutex2);
	//while(!condition)
    if(l2==1)
    {
        l2 = 2;
        pthread_cond_wait(&cond2, &mutex2);
        clock_t t;
        t = clock();
        printf("Child 2 running\n");
        
        FILE *fp;
        if((fp=fopen("second.txt", "r"))==NULL)
        {
        	printf("Error: Cannot open the file\n");
        	exit(EXIT_FAILURE);
        }
        else
        {
        	printf("Numbers in the file are: ");
        	char num[10];
        	
        	for(int i=1; i<= *param; i++)
        	{
        		fgets(num, 100, fp);
        		int val = atoi(num);
        		printf("%d\t", val);
        	}
        	printf("\n");
        	
        }
        fclose(fp);
        printf("Child 2 completed\n");
        t = clock() - t;
        double tt = ((double)t);
        printf("Child 2 completed after %lf microseconds\n", tt);
    }
    else
    {
        //Child 2 montior thread
        while(!(!shmPtr[0] && shmPtr[1]))
        {  
                usleep(20);
        }
        pthread_cond_signal(&cond2);
        printf("monitor thread 2 completed\n");
    }
    
	//do_reading of n2 numbers from input file
	//when done,print done_reading on the console
	pthread_mutex_unlock(&mutex2);
 	// printf("Child 2 completed\n");
}

void *read_add_nums(void *p){
    int *param = (int *)p;
 	pthread_mutex_lock(&mutex3);
	//while(!condition)
    if(l3==1)
    {
        l3 = 2;
        pthread_cond_wait(&cond3, &mutex3);
        clock_t t;
        t = clock();
        printf("Child 3 running\n");
        FILE *fp;
        long long sum = 0;
        
        if((fp=fopen("third.txt", "r"))==NULL)
        {
        	printf("Error: Cannot open the file\n");
        	exit(EXIT_FAILURE);
        }
        else
        {
        	char num[10];
        	for(int i=1; i<= *param; i++)
        	{
        		fgets(num, 100, fp);
        		int val = atoi(num);
        		sum += val;
        	}
        }
        fclose(fp);
        printf("Child 3 completed\n");
        t = clock() - t;
        double tt = ((double)t);
        printf("Child 3 completed after %lf microseconds\n", tt);
        return (void *)sum;
    }
    else
    {
        //Child 3 montior thread
        while(!(!shmPtr[0] && !shmPtr[1]))
        {  
                usleep(20);
        }
        pthread_cond_signal(&cond3);
        printf("monitor thread 3 completed\n");
    }
    
	//do_reading of n2 numbers from input file
	//when done,print done_reading on the console
	pthread_mutex_unlock(&mutex3);
 	
 	
}




void roundrobin(int n1,int n2,int n3){
	pid_t child_c1,child_c2,child_c3;
 	int pfd1[2];
 	int p1 = pipe(pfd1);
 	child_c1 = fork();
	if(child_c1 == 0)
    { 
        //Child_c1 process
        //create 2 threads here  
    }
    else
    { 
        //Parent process
        int pfd2[2];
        int p2 = pipe(pfd2);
        child_c2 = fork();
        if(child_c2 == 0)
        {   
            //Child_c2 process
            //create 2 threads here  


        }
        else
        {   
            //Parent process
            int pfd3[2];
            int p3 = pipe(pfd3);
            child_c3 = fork();
            if(child_c3 == 0)
            {   
                //Child_c3 process
                //create 2 threads here  

            }
            else
            {
                //parent process
                wait(NULL);
                wait(NULL);
                wait(NULL);
            }


                

        }
    }
}

void fcfs(int n1,int n2,int n3){

    // printf("inside fcfs function\n");

    pthread_mutex_init(&mutex1,NULL);
    
    int shmid; 
    //int n;

	pthread_t computation_thread[3];
	pthread_t monitor_thread[3];

	pthread_attr_t at[6];
	for(int i = 0; i < 6; i++)
		pthread_attr_init(&at[i]);

    //shmPtr[0] = bool for c1
    //shmPtr[1] = bool for c2
   

	pid_t child_c1,child_c2,child_c3;
 	int pfd1[2];
 	int p1 = pipe(pfd1);
 	child_c1 = fork();
	if(child_c1 == 0)
    	{ 
        //Child_c1 process
        
        sleep(1);
        // printf("child1\n");
        if( (shmid = shmget(2041, 32, 0)) == -1 )
        {
            perror("error in shmget in child1");
            exit(1); 
        }
        shmPtr = shmat(shmid, 0, 0); //attaching to shared memory
        if (shmPtr == (bool*) -1)
        {
            perror("error in shmat in child1");
            exit(2);
        }
        
        // printf("before thread creation c1\n");
        
	void *result1;
        
        pthread_create(&computation_thread[0], &at[0], add_nums, &n1);
        usleep(200);
        pthread_create(&monitor_thread[0], &at[1], add_nums, &n1);
        // printf("after thread creation c1\n");
        pthread_join(computation_thread[0], &result1);
        pthread_join(monitor_thread[0], NULL);
        long long result = (long long)result1;
        
        //passing the result to parent through pipe pfd1
	close(pfd1[0]);
	write(pfd1[1], &result, sizeof(result));
	close(pfd1[1]);
       

    }
    else
    {   
        //Parent process
        int pfd2[2];
        int p2 = pipe(pfd2);
        child_c2 = fork();
            if(child_c2 == 0)
            { 
                //Child_c2 process
                
                sleep(1);
                // printf("child2\n");
                if( (shmid = shmget(2041, 32, 0)) == -1 )
                {
                    perror("error in shmget in child1");
                    exit(1); 
                }
                shmPtr = shmat(shmid, 0, 0); //attaching to shared memory
                if (shmPtr == (bool*) -1)
                {
                    perror("error in shmat in child1");
                    exit(2);
                }
                // printf("before thread creation c2\n");
                pthread_create(&monitor_thread[1], &at[3], read_nums, &n2);
                pthread_create(&computation_thread[1], &at[2], read_nums, &n2);
                // printf("after thread creation c2\n");
                pthread_join(computation_thread[1], NULL);
                pthread_join(monitor_thread[1], NULL);
                
                //passing message to parent through pfd2
                close(pfd2[0]);
                write(pfd2[1], "Done Printing", 14);
                close(pfd2[1]);

               
            }
            else
            { 
                //Parent process
                int pfd3[2];
                int p3 = pipe(pfd3);
                child_c3 = fork();
                if(child_c3 == 0)
                {   
                    //Child_c3 process
                    
                    sleep(1);
                    // printf("child3\n");
                    if( (shmid = shmget(2041, 32, 0)) == -1 )
                    {
                        perror("error in shmget in child1");
                        exit(1); 
                    }
                    shmPtr = shmat(shmid, 0, 0); //attaching to shared memory
                    if (shmPtr == (bool*) -1)
                    {
                        perror("error in shmat in child1");
                        exit(2);
                    }

                    // printf("before thread creation c2\n");

			pthread_create(&computation_thread[2], &at[4], read_add_nums, &n3);
			usleep(200);
                    pthread_create(&monitor_thread[2], &at[5], read_add_nums, &n3);
					
                	// printf("after thread creation c3\n");
                   
                   void *result2;
					pthread_join(computation_thread[2], &result2);
         			pthread_join(monitor_thread[2], NULL);
                  long long result3 = (long long)result2;
                  
                  //passing result from child 3 to parent through pfd3 pipe
                  close(pfd3[0]);
                  write(pfd3[1], &result3, sizeof(result3));
                  close(pfd3[1]);
                  
                }
                else
                {
                    //parent process
                    printf("parent\n");

                    if( (shmid = shmget(2041, 32, 0666 | IPC_CREAT)) == -1 )
                    {
                        perror("error in shmget in parent");
                        exit(1);
                    }
                    // printf("workd till here\n");
                    shmPtr = shmat(shmid, 0, 0);
                    if (shmPtr == (bool *) -1)
                    {
                        perror("error in shmat in parent");
                        exit(2);
                    }
                    // printf("works till here\n");
                    printf("Parent writing\n");
                    //fcfs scheduling here
                    
                    shmPtr[0] = true;
                    shmPtr[1] = false;
                    //receiving result from child1 through pipe pfd1
                    long long buf;
                    close(pfd1[1]);
                    read(pfd1[0], &buf, 64);
                    close(pfd1[0]);
                    printf("Result of Child 1 : %lld\n", buf);
                    // printf("over here\n");
                    wait(NULL);

                    shmPtr[0] = false;
                    shmPtr[1] = true;
                    //receiving message from child2 through pipe pfd2
                    char buf1[14];
                    close(pfd2[1]);
                    read(pfd2[0], buf1, 14);
                    printf("Child 2 has sent the message: \"%s\"\n", buf1);
                    close(pfd2[0]);
                    wait(NULL);

                    shmPtr[0] = false;
                    shmPtr[1] = false;
                    //receiving message from child3 through pipe pfd3
                    long long buf2;
                    close(pfd3[1]);
                    read(pfd3[0], &buf2, 64);
                    close(pfd3[0]);
                    printf("Result of Child 3 : %lld\n", buf2);
                    wait(NULL);
                    
                    //fcfs scheduling here

                    // wait(NULL);
                    // wait(NULL);
                    // wait(NULL);

                    //closing the shared memory space
                    if( shmctl(shmid, IPC_RMID, NULL) == -1 )
                    {
                        perror("error in shmctl");
                        exit(-1);
                    }
                }
            }
    }

}

void work(int scheduling,int n1,int n2,int n3){
    if(scheduling == 1)
        roundrobin(n1,n2,n3);
    else
        fcfs(n1,n2,n3);
}


int main(){
//input from user : choice of algo,n1,n2,n3
//shared memory setup
    int scheduling;
    printf("Choose desired scheduling algorithm :\n");
    printf("Enter 1 for Round Robin\n");
    printf("Enter 2 for FCFS\n");
    scanf("%d",&scheduling);
    int n1,n2,n3;
    printf("Enter workload for child processes N1,N2,N3\n");
    scanf("%d%d%d",&n1,&n2,&n3);
    work(scheduling,n1,n2,n3);    
}
