#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <sys/time.h>

#include <pthread.h>

#include "utils.h"

struct SumArgs {
  int *array;
  int begin;
  int end;
};

long long Sum(const struct SumArgs *args) {
  long long sum = 0;
  for (int i = args->begin; i < args->end; i++) {
    sum += args->array[i];
  }
  return sum;
}

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int threads_num = -1;

  while (true) {
    int current_optind = optind ? optind : 1;
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0: seed = atoi(optarg); break;
          case 1: array_size = atoi(optarg); break;
          case 2: threads_num = atoi(optarg); break;
          default: printf("Index %d is out of options\n", option_index); return 1;
        }
        break;
      case '?': return 1;
      default: printf("getopt returned character code 0%o?\n", c); return 1;
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || threads_num == -1 || threads_num <= 0) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\"\n", argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  if (!array) return 1;
  GenerateArray(array, array_size, seed);

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  pthread_t *threads = malloc(sizeof(pthread_t) * threads_num);
  struct SumArgs *args = malloc(sizeof(struct SumArgs) * threads_num);

  int chunk_size = array_size / threads_num;
  for (uint32_t i = 0; i < threads_num; i++) {
    args[i].array = array;
    args[i].begin = i * chunk_size;
    args[i].end = (i == threads_num - 1) ? array_size : args[i].begin + chunk_size;

    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      free(array);
      free(threads);
      free(args);
      return 1;
    }
  }

  long long total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    long long sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  free(threads);
  free(args);
  
  printf("Total: %lld\n", total_sum);
  printf("Elapsed time: %fms\n", elapsed_time);
  return 0;
}