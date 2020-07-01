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

struct Server {
  char ip[255];
  int port;
};

struct ServerArgs
{
  struct Server to;
  int begin;
  int end;
  int mod;
};

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
  uint64_t result = 0;
  a = a % mod;
  while (b > 0) {
    if (b % 2 == 1)
      result = (result + a) % mod;
    a = (a * 2) % mod;
    b /= 2;
  }
  return result % mod;
}

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

uint64_t To_Server(const struct ServerArgs *arg)
{
  struct Server to = arg->to;
  uint64_t begin = arg->begin;
  uint64_t end = arg->end;
  uint64_t mod = arg->mod;

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

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
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
    printf("answer: %llu\n", answer);

    return answer;
}


void *ThreadServer(void *input) {
  struct ServerArgs *arg = (struct ServerArgs *)input;
  return (void *)(size_t)To_Server(arg);
}

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
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
        if(k <= 0)
            {
              printf("number should be positive");
              return 1;
            }
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        if(mod <= 0)
            {
              printf("mod should be positive");
              return 1;
            }
        break;
      case 2:
        if(strlen(optarg) > 255)
            {
              printf("len of path should be shotter 255");
              return 1;
            }
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
  int counter = 0;
  fp = fopen(servers, "rb");
  fseek (fp, 0, SEEK_END);
  int size1 = ftell(fp);
  char *str = (char*)malloc(size1);
  char *str_copy = (char*)malloc(size1);
  fseek (fp, 0, SEEK_SET);

  for(int i=0; i<size1; i++)
  {
    str[i]=fgetc(fp);
  }
  fclose(fp);
  strcpy(str_copy,str);
  char* timed;
  
  timed = strtok(str_copy," :");
  unsigned int servers_num = 0;
  while(timed)
  {
    timed = strtok(NULL," :");
    servers_num += 1;
  }
  free(str_copy);
  servers_num = servers_num / 2;
  struct Server *to = malloc(sizeof(struct Server) * servers_num);
  struct ServerArgs *mass = malloc(sizeof(struct ServerArgs)*servers_num);
  timed = strtok(str," :");
  for(int i=0; i< servers_num; i++)
  {
    strcpy(to[i].ip, timed);
    to[i].port = atoi(strtok(NULL," :"));
    mass[i].to = to[i];
    mass[i].mod = mod;
    timed = strtok(NULL," :");
  }
  free(str);
  mass[0].begin = 1;
  for(int i=0; i < servers_num-1; i++)
  {
    mass[i].end = mass[i].begin + k/servers_num;
    mass[i+1].begin= mass[i].end + 1;
  }
  mass[servers_num-1].end = k;

  pthread_t threads[servers_num];
  for (uint32_t i = 0; i < servers_num; i++){
    if (pthread_create(&threads[i], NULL, ThreadServer, (void *)&mass[i]))
    {
      printf("Error: pthread_create failed!\n");
      return 1;
    }
  }

  int middl[servers_num];
  for (uint32_t i = 0; i < servers_num; i++) {
    pthread_join(threads[i], (void **)&middl[i]);
  }

  int total_fact = 1;
  for (uint32_t i = 0; i < servers_num; i++) {
    total_fact = MultModulo(total_fact, middl[i], mod);
  }

  printf("Total: %ld\n", total_fact);
  free(to);
  return 0;
}
