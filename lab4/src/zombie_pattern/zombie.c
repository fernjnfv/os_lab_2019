#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
main()
{
    int pid;
    pid=fork();
    if(pid==0)
    {
        printf("\nI am the child\n");
        sleep(5);
        exit(0);
    }
    if(pid>0)
    {
        printf("\nI am the parent\n");
        //pid become zombie without the code below
        //cause while the main process is dead,
        //pid is still alive and continue working
        int id;
        wait(&id);
    }
}