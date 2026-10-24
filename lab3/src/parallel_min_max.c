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
#include <getopt.h>
#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;
    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};
    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);
    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0: seed = atoi(optarg); break;
          case 1: array_size = atoi(optarg); break;
          case 2: pnum = atoi(optarg); break;
          case 3: with_files = true; break;
          default: printf("Index %d is out of options\n", option_index); return 1;
        }
        break;
      case 'f': with_files = true; break;
      case '?': return 1;
      default: printf("getopt returned character code 0%o?\n", c); return 1;
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1 || pnum <= 0) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [-f]\n", argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  if (!array) return 1;
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int pipes[pnum][2];
  if (!with_files) {
    for (int i = 0; i < pnum; i++) {
      pipe(pipes[i]);
    }
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      active_child_processes += 1;
      if (child_pid == 0) {
        int chunk_size = array_size / pnum;
        int start_index = i * chunk_size;
        int end_index = (i == pnum - 1) ? array_size : start_index + chunk_size;

        int min = INT_MAX;
        int max = INT_MIN;
        for (int j = start_index; j < end_index; j++) {
          if (array[j] < min) min = array[j];
          if (array[j] > max) max = array[j];
        }

        if (with_files) {
          char filename[256];
          snprintf(filename, sizeof(filename), "/tmp/minmax_%d.txt", i);
          FILE *file = fopen(filename, "w");
          fprintf(file, "%d %d\n", min, max);
          fclose(file);
        } else {
          close(pipes[i][0]);
          write(pipes[i][1], &min, sizeof(int));
          write(pipes[i][1], &max, sizeof(int));
          close(pipes[i][1]);
        }
        return 0;
      }
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (!with_files) {
    for (int i = 0; i < pnum; i++) {
      close(pipes[i][1]);
    }
  }

  while (active_child_processes > 0) {
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
       printf("с файлами");
      char filename[256];
      snprintf(filename, sizeof(filename), "/tmp/minmax_%d.txt", i);
      FILE *file = fopen(filename, "r");
      if (file) {
        fscanf(file, "%d %d", &min, &max);
        fclose(file);
        remove(filename);
      }
    } else {
      read(pipes[i][0], &min, sizeof(int));
      read(pipes[i][0], &max, sizeof(int));
      close(pipes[i][0]);
    }

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