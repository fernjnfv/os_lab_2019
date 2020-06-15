#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

struct Server {
  char ip[255];
  int port;
};

struct Task
{
  int begin_num;
  int end_num;
};

struct ServerArgs
{
  struct Server to;
  struct Task task;
};

uint64_t mod;

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

uint64_t AskServ(const struct ServerArgs *arg) {
  struct Server to = arg->to;
  struct Task task = arg->task;
  struct hostent *hostname = gethostbyname(to.ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", to.ip);
    exit(1);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(to.port);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    fprintf(stderr, "Socket creation failed!\n");
    exit(1);
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Connection failed\n");
    exit(1);
  }

  uint64_t begin = task.begin_num;
  uint64_t end = task.end_num;

  char job[sizeof(uint64_t) * 3];
  memcpy(job, &begin, sizeof(uint64_t));
  memcpy(job + sizeof(uint64_t), &end, sizeof(uint64_t));
  memcpy(job + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

  if (send(sck, job, sizeof(job), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }

  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));
  printf("From server: %llu\n", answer);

  return answer;
  close(sck);
  return 1;
}

void *ThreadServer(void *input) {
  struct ServerArgs *arg = (struct ServerArgs *)input;
  return (void *)(size_t)AskServ(arg);
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        break;
      case 2:
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }
  
  FILE *fp;
  char c[255];
  int counter = 0;
  
  fp = fopen(servers, "r");
  while(true)
  {
    c[counter] = fgetc(fp);
    if(feof(fp))
    {
      c[counter] = '\0';
      break;
    }
    ++counter;
  }
  fclose(fp);

  char copy_str[255];
  strcpy(copy_str, c);

  unsigned int servers_num = 0;
  char* pch;
  pch = strtok(copy_str," :");
  while (pch != NULL)
  {
    ++servers_num;

    pch = strtok(NULL, " :");
  }
  servers_num /= 2;

  uint64_t port;
  struct Server *to = malloc(sizeof(struct Server) * servers_num);

  int counter_a, counter_b;
  counter_a = counter_b = 0;
  bool iter = 0;
  pch = strtok(c," :");
  while (pch != NULL)
  {
    if (iter == 1){
      port = atoi(pch);
      to[counter_a].port = port;
      counter_a++;
    }
    else{
      memcpy(to[counter_b].ip, "127.0.0.1", sizeof("127.0.0.1"));
      counter_b++;
    }
    
    pch = strtok(NULL, " :");
    iter = !iter;
  }

  free(pch);


  //let servers_num be 3 and k be 20
  //part size be 6
  //fill parts with 1-6 7-12 13-20
  struct Task tasks[servers_num];
  int part_size = k/servers_num;
  tasks[0].begin_num = 1;
  tasks[servers_num-1].end_num = k;
  for(int i = 0; i < servers_num - 1; i++)
  {
    tasks[i].end_num = tasks[i].begin_num + part_size;
    if (i+1 < servers_num)
    {
      tasks[i+1].begin_num = tasks[i].end_num + 1;
    }
  }

  struct ServerArgs s_args[servers_num];
  for (int i = 0; i < servers_num; i++){
    s_args[i].task = tasks[i];
    s_args[i].to = to[i];
  }

  pthread_t threads[servers_num];
  for (uint32_t i = 0; i < servers_num; i++){
    if (pthread_create(&threads[i], NULL, ThreadServer, (void *)&s_args[i]))
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int facts[servers_num];
  for (uint32_t i = 0; i < servers_num; i++) {
    pthread_join(threads[i], (void **)&facts[i]);
  }

  int total_fact = 1;
  for (uint32_t i = 0; i < servers_num; i++) {
    total_fact *= facts[i];
    total_fact %= mod;
  }

  printf("Total: %ld\n", total_fact);

  free(to);

  return 0;
}
