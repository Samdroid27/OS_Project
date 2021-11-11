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

#define READ 0
#define WRITE 1

int main(){
 pid_t child_c1,child_c2,child_c3;
 int pfd1[2];
 int p1 = pipe(pfd1);
 child_c1 = fork();
//input from user : choice of algo,n1,n2,n3 and communicate n1 to c1, n2 to c2 , n3 to c3 using pipes
//shared memory setup

    if(child_c1 == 0)
        { //Child_c1 process
          //create 2 threads here  
        }
    else
        { //Parent process
        
            int pfd2[2];
            int p2 = pipe(pfd2);
            child_c2 = fork();
                if(child_c2 == 0)
                { //Child_c2 process
                  //create 2 threads here  


                }
                else
                { //Parent process
                    int pfd3[2];
                    int p3 = pipe(pfd3);
                    child_c3 = fork();
                        if(child_c3 == 0)
                        { //Child_c3 process
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