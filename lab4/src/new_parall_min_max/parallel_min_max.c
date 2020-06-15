#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <signal.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

bool do_kill = false;

void handler(int signal){
  //printf("The execution time is expired, kill all child processes\n");
  if(signal == SIGALRM){
    do_kill = true;
  }
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;
    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            break;
          case 3:
            timeout = atoi(optarg);
            break;
          case 4:
            with_files = true;
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int** pipes_max = malloc(sizeof(int*) * pnum);
  int** pipes_min = malloc(sizeof(int*) * pnum);

  for (int i = 0; i < pnum; i++) {
      //init pipe
      int p_max[2];
      int p_min[2];
      //check if pipe fail
      if (pipe(p_max) == -1){
        printf("Pipe Failed");
        return 1;
      }
      if (pipe(p_min) == -1){
        printf("Pipe Failed");
        return 1;
      }
      pipes_max[i] = p_max;
      pipes_min[i] = p_min;
  }

  int arr_part_size = array_size/pnum;
  int arr_parts[pnum];
  int pointer = 0;
  //let pnum be 3 and arr size is 20
  //part size be 6
  //fill parts with 0 6 12
  arr_parts[0] = 0;
  for(int i = 1; i < pnum; i++){
    arr_parts[i] = arr_parts[i-1] + arr_part_size;
  }

  FILE* f;
  f = fopen("buff", "w");

  int* pid_list = malloc(sizeof(int) * pnum);

  if (timeout > 0) {
    alarm(timeout);
    signal(SIGALRM, handler);
  }

  for (int i = 0; i < pnum; i++) {
    //pid is > 0 for main process and 0 for child
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      printf("Process is %d\n", child_pid);
      if(child_pid > 0){
        pid_list[i] = child_pid;
      }

      if (child_pid == 0) {
        // child process - parallel code here

        //prolong this process a little
        sleep(5);
        
        int max = INT_MIN;
        int min = INT_MAX;

        //find min/max
        //every thread calculate its own part of array
        //if arr have odd size give rest of it to procs_num-1 proc
        if(i != pnum-1){
          for(int j = arr_parts[i]; j < arr_parts[i+1]; j++){
            if(max < array[j]){
              max = array[j];
            }
            if(min > array[j]){
              min = array[j];
            }
          }
        }
        else{
          for(int j = arr_parts[i]; j < array_size; j++){
            if(max < array[j]){
              max = array[j];
            }
            if(min > array[j]){
              min = array[j];
            }
          }
        }

        if (with_files) {
          char name[6];
          name[0] = 'b'; name[1] = 'u';
          name[2] = 'f'; name[3] = 'f';
          name[4] = i + '0'; name[5] = '\0';

          fprintf(f,"%d %d ", max, min);
          fclose(f);
        }
        else {
          close(pipes_max[i][0]);
          close(pipes_min[i][0]);

          //write min/max
          write(pipes_min[i][1], &min, sizeof(int));
          write(pipes_max[i][1], &max, sizeof(int));

          close(pipes_max[i][1]);
          close(pipes_min[i][1]);
        }
        //printf("I did it\n");
        return 0;
      }
    }
    else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (timeout > 0) {
    int status = -1;
    while(!do_kill);
      for(int i = 0; i < pnum; i++){
        int code = waitpid(pid_list[i], &status, 1);
        if (!WIFEXITED(status)){
          kill(pid_list[i], SIGKILL);
          printf("Killed, %d cause time is expired\n", pid_list[i]);
        }
    }
  }

  //output of programm
  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  f = fopen("buff", "r");
  for (int i = 0; i < pnum; i++) {
    int max = INT_MIN;
    int min = INT_MAX;

    //there we set our min/max nums
    if (with_files) {
      while(wait(NULL)>0);
      fscanf(f, "%d", &max);
      fscanf(f, "%d", &min);
    }
    else {
      close(pipes_max[i][1]);
      close(pipes_min[i][1]);

      read(pipes_max[i][0],&max,sizeof(int));
      read(pipes_min[i][0],&min,sizeof(int));
    }

    //there we fill the struture of min/max nums
    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
