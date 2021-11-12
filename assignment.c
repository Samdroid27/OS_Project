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

#define READ 0
#define WRITE 1

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
    
    int shmid; 
    bool *shmPtr; 
    int n;

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

        //if(shmPtr[0] && !shmPtr[1]) then child 1 runs
        while(!(shmPtr[0] && !shmPtr[1]));
        
        {
            printf("Child1 running\n");
        }

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
                sleep(1);
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

                //if(!shmPtr[0] && shmPtr[1]) then child 2 runs
                while(!(!shmPtr[0] && shmPtr[1]));

                {
                    printf("Child2 running\n");
                }

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
                    sleep(1);
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

                    //if(!shmPtr[0] && !shmPtr[1]) then child 3 runs
                    while(!(!shmPtr[0] && !shmPtr[1]));
                    
                    {
                        //child 3 code here
                        printf("Child3 running\n");
                    }

                    //create 2 threads here  
                }
                else
                {
                    //parent process

                    if( (shmid = shmget(2041, 32, 0666 | IPC_CREAT)) == -1 )
                    {
                        perror("error in shmget in parent");
                        exit(1);
                    }
                    shmPtr = shmat(shmid, 0, 0);
                    if (shmPtr == (bool *) -1)
                    {
                        perror("error in shmat in parent");
                        exit(2);
                    }

                    //fcfs scheduling here
                    printf("Parent writing");
                    shmPtr[0] = true;
                    shmPtr[1] = false;
                    wait(NULL);

                    shmPtr[0] = false;
                    shmPtr[1] = true;
                    wait(NULL);

                    shmPtr[0] = false;
                    shmPtr[1] = false;

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
