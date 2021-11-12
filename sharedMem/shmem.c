#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

int main()
{
        int shmid; 
        bool *shmPtr; 
        int n;
        
        
        if(fork() == 0) {
                sleep(5); /* To wait for the parent to write */
        
                if( (shmid = shmget(2041, 32, 0)) == -1 )
                {
                    perror("error in shmget in child");
                    exit(1); 
                }
                
                shmPtr = shmat(shmid, 0, 0); //attaching to shared memory
                
                if (shmPtr == (bool*) -1)
                {
                    perror("error in shmat in child");
                    exit(2);
                }
                printf ("\nChild Reading ....\n\n");
                
                for (n = 0; n < 26; n++)
                {
                        if(shmPtr[n])
                                printf("True\n");
                        else
                                printf("False\n");
                }
                // putchar('\n');
        } 
        else {
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
                for (n = 0; n < 26; n++)
                        shmPtr[n] = n%2;
                printf ("\nParent Writing ....\n\n") ;
                for (n = 0; n < 26; n++)
                {
                        if(shmPtr[n])
                                printf("True\n");
                        else
                                printf("False\n");
                }
                // putchar('\n');
                wait(NULL);
                if( shmctl(shmid, IPC_RMID, NULL) == -1 ){
                        perror("shmctl");
                        exit(-1);
                }
        }
        return 0;
}
