#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
int main ()
{
  pid_t child_pid;
child_pid = fork ();
  if (child_pid > 0) {
    sleep (5);
  }
  else {
    printf("я стал зомби\n");
    exit (0);
    
  }
  return 0;
}

