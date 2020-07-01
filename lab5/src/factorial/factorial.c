#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
 int mod = -1;
 unsigned long long total_fact_global = 1;

struct FactArgs {
  int begin_num;
  int end_num;
};

void *ThreadFact(void *arg) {

  struct FactArgs *fact_arg = (struct FactArgs *)arg;

  unsigned long long fact = 1;
  for(int i = fact_arg->begin_num; i < fact_arg->end_num; i++){
    fact *= (i + 1);
    fact %= mod;
  }

  pthread_mutex_lock(&mut);
  total_fact_global *= fact;
  pthread_mutex_unlock(&mut);
}

int main(int argc, char **argv) {
  int k = -1;
  int pnum = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
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
            k = atoi(optarg);
            if(k <= 0)
            {
              printf("number should be positive");
              return 1;
            }
            break;
          case 1:
            pnum = atoi(optarg);
            if(pnum <= 0)
            {
              printf("pnum should be positive");
              return 1;
            }
            break;
          case 2:
            mod = atoi(optarg);
            if(mod <= 0)
            {
              printf("mod should be positive");
              return 1;
            }
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
  if (k == -1 || pnum == -1 || mod == -1) {
    printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\" \n",
           argv[0]);
    return 1;
  }

  pthread_t threads[pnum];
  
  struct FactArgs args[pnum];
  
  args[0].begin_num = 0;
  for(int i = 0; i < pnum - 1; i++)
  {
    args[i].end_num = args[i].begin_num + k/pnum;
    args[i+1].begin_num = args[i].end_num;
  }
  args[pnum-1].end_num = k;

  for (int i = 0; i < pnum; i++){
    if (pthread_create(&threads[i], NULL, ThreadFact, (void *)&args[i]))
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  for (int i = 0; i < pnum; i++) {
    pthread_join(threads[i], 0);
  }
  total_fact_global %= mod;

  printf("Total factorial by mutex: %lld\n", total_fact_global);
  return 0;
}