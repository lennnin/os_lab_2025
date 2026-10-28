#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <getopt.h>
#include <pthread.h>

long long global_result = 1;
long long mod_value = 1;
pthread_mutex_t mutex;

struct FactorialArgs {
  long long begin;
  long long end;
};

void *ThreadFactorial(void *args) {
  struct FactorialArgs *f_args = (struct FactorialArgs *)args;
  long long partial = 1;

  for (long long i = f_args->begin; i <= f_args->end; i++) {
    printf("настройки :  %lld  %lld\n", partial % mod_value, mod_value);
    partial = (partial * i) % mod_value;
  }

  pthread_mutex_lock(&mutex);
  global_result = (global_result * partial) % mod_value;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

int main(int argc, char **argv) {
  long long k = -1;
  int pnum = -1;
  long long mod = -1;

  while (true) {
    static struct option options[] = {{"k", required_argument, 0, 'k'},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "k:", options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 'k':
        k = atoll(optarg);
        break;
      case 0:
        switch (option_index) {
          case 1: pnum = atoi(optarg); break;
          case 2: mod = atoll(optarg); break;
          default: printf("Index %d is out of options\n", option_index); return 1;
        }
        break;
      case '?': return 1;
      default: printf("getopt returned character code 0%o?\n", c); return 1;
    }
  }

  if (k == -1 || pnum == -1 || mod == -1 || pnum <= 0 || k < 0 || mod <= 0) {
    printf("Usage: %s -k <num> --pnum=<num> --mod=<num>\n", argv[0]);
    return 1;
  }

  if (k == 0 || k == 1) {
    printf("Result: %lld\n", 1 % mod);
    return 0;
  }

  mod_value = mod;
  pthread_mutex_init(&mutex, NULL);

  int actual_pnum = pnum;
  if (k < pnum) {
    actual_pnum = k;
  }

  pthread_t *threads = malloc(sizeof(pthread_t) * actual_pnum);
  struct FactorialArgs *args = malloc(sizeof(struct FactorialArgs) * actual_pnum);

  long long chunk_size = k / actual_pnum;
  for (int i = 0; i < actual_pnum; i++) {
    printf("настройки : %lld  %d   %lld\n", k, pnum, mod);
    args[i].begin = i * chunk_size + 1;
    args[i].end = (i == actual_pnum - 1) ? k : (i + 1) * chunk_size;

    if (pthread_create(&threads[i], NULL, ThreadFactorial, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      free(threads);
      free(args);
      pthread_mutex_destroy(&mutex);
      return 1;
    }
  }

  for (int i = 0; i < actual_pnum; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("Result: %lld\n", global_result);

  pthread_mutex_destroy(&mutex);
  free(threads);
  free(args);
  return 0;
}