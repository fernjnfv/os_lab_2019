#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <stdint.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>

typedef unsigned long long ull;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int mod = -1;
ull total_fact_global = 1;

struct FactArgs {
  int begin_num;
  int end_num;
};

ull Fact(const struct FactArgs *arg) {
  ull calk = 1;
  for(int i = arg->begin_num; i < arg->end_num; i++){
    calk *= (i + 1) % mod;
    //printf("From %d %d: %lld\n", arg->begin_num, arg->end_num, calk);
  }
  return calk;
}

void *ThreadFact(void *arg) {
  struct FactArgs *fact_arg = (struct FactArgs *)arg;

  ull fact = Fact(fact_arg);

  pthread_mutex_lock(&mut);
  total_fact_global *= fact;
  pthread_mutex_unlock(&mut);

  //return (void *)Fact(fact_arg);
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
            break;
          case 1:
            pnum = atoi(optarg);
            break;
          case 2:
            mod = atoi(optarg);
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
  //let pnum be 3 and k be 20
  //part size be 6
  //fill parts with 0 6 12 20
  struct FactArgs args[pnum];
  int part_size = k/pnum;
  args[0].begin_num = 0;
  args[pnum-1].end_num = k;
  for(int i = 0; i < pnum - 1; i++)
  {
    args[i].end_num = args[i].begin_num + part_size;
    if (i+1 < pnum)
    {
      args[i+1].begin_num = args[i].end_num;
    }
  }

  for (int i = 0; i < pnum; i++){
    if (pthread_create(&threads[i], NULL, ThreadFact, (void *)&args[i]))
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  ull total_fact = 1;
  for (int i = 0; i < pnum; i++) {
    //ull fact = 0;
    pthread_join(threads[i], 0);
    //printf("Part of factorial from %d to %d is: %lld\n",
    //        args[i].begin_num, args[i].end_num, fact);
    //total_fact *= fact;
    //printf("Now total factorial is: %lld\n", total_fact);
  }
  //total_fact %= mod;
  total_fact_global %= mod;

  //printf("Total factorial: %lld\n", total_fact);
  printf("Total factorial by mutex: %lld\n", total_fact_global);
  return 0;
}