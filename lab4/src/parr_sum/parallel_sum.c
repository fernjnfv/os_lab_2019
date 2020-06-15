#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <getopt.h>

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

struct SumArgs {
  int *array;
  int begin;
  int end;
};

void GenerateArray(int *array, unsigned int array_size, unsigned int seed) {
  srand(seed);
  for (int i = 0; i < array_size; i++) {
    array[i] = rand() % 101;
  }
}

int Sum(const struct SumArgs *arg) {
  int sum = 0;
  for(int i = arg->begin; i < arg->end; i++){
    sum += arg->array[i];
  }
  return sum;
}

void *ThreadSum(void *arg) {
  struct SumArgs *sum_arg = (struct SumArgs *)arg;
  return (void *)(size_t)Sum(sum_arg);
}

int main(int argc, char **argv) {
  uint32_t threads_num = -1;
  uint32_t array_size = -1;
  uint32_t seed = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"threads_num", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"seed", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1) break;
    switch (c)
    {
      case 0:
        switch (option_index)
        {
          case 0:
            threads_num = atoi(optarg);
            break;
          case 1:
            array_size = atoi(optarg);
            break;
          case 2:
            seed = atoi(optarg);
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
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
  if (seed == -1 || array_size == -1 || threads_num == -1) {
    printf("Usage: %s --threads_num \"num\" --array_size \"num\" --seed \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  pthread_t threads[threads_num];

  struct SumArgs args[threads_num];

  for (int i = 0; i < threads_num; i++){
    args[i].array = array;
  }

  //let threads_num be 3 and arr size is 20
  //part size be 6
  //fill parts with 0 6 12 20
  int arr_part_size = array_size/threads_num;
  args[0].begin = 0;
  args[threads_num-1].end = array_size;
  for(int i = 0; i < threads_num - 1; i++){
    args[i].end = args[i].begin + arr_part_size;
    if (i+1 < threads_num){
      args[i+1].begin = args[i].end;
    }
  }

  for (uint32_t i = 0; i < threads_num; i++){
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i]))
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    printf("Part of summ is: %d\n", sum);
    total_sum += sum;
  }

  free(array);

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);
  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  printf("Elapsed time: %fms\n", elapsed_time);

  printf("Total: %d\n", total_sum);
  return 0;
}
