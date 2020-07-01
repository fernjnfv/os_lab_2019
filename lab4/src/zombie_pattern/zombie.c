#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include <stdbool.h>

int main()
{
    int forkers = 0;
    while (true)
    {
    forkers = forkers + 1;
    pid_t pid = fork();
    if(pid==0)
    {
        //printf("\nI am zombie");
        exit(0);
    }
    if(pid<0)
    {
        //printf("\nzombie everywhere, but i'll check");
        pid = fork();
        if(pid==0)
    {
        //printf("\nI am new zombie");
        exit(0);
    }
    if(pid<0)
        break;
    }
    }
        printf("\nthere %d zombie, and we can't fork anymore\n",forkers);
        printf("\nBut I am the parent. I going to kill them\n");
        while(forkers > 1)
        {
        while(wait(NULL) > 0);
        forkers = forkers -1;
        }
        printf("They dye, again\n");
        pid_t pid  = fork();
        if(pid==0)
    {
        printf("\nWe can fork again\n");
        exit(0);
    }
    if(pid<0)
    {
        printf("\nsomething wrong\n");
    }
}