#include <stdio.h>
#include <unistd.h>
int main(int argc, char** argv){
    execv("sequential_min_max", argv);
}