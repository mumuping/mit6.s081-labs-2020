#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#define READ_PIPE 0
#define WRITE_PIPE 1

/**
 * Write a concurrent version of prime sieve using pipes. This idea is due to
 * Doug McIlroy, inventor of Unix pipes. The picture halfway down this
 * page(https://swtch.com/~rsc/thread/) and the surrounding text explain how to
 * do it. Your solution should be in the file user/primes.c.
 *
 * Your goal is to use pipe and fork to set up the pipeline. The first process
 * feeds the numbers 2 through 35 into the pipeline. For each prime number, you
 * will arrange to create one process that reads from its left neighbor over a
 * pipe and writes to its right neighbor over another pipe. Since xv6 has
 * limited number of file descriptors and processes, the first process can stop
 * at 35.
 */

/**
 * @brief Create a child process
 * @param left_pipe: the pipe from left neighbor
 */
void child_process(int left_pipe[]) {
  /* Init the child process */
  close(left_pipe[WRITE_PIPE]);
  /* Read from the left pipe */
  int prime;
  /* It will be blocked if there is no data in the pipe */
  int state = read(left_pipe[READ_PIPE], &prime, sizeof(int));
  if (state <= 0) {
    exit(0);
  }

  /* Print the prime */
  printf("prime %d\n", prime);

  /* Create a new pipe for the right neighbor */
  int right_pipe[2];
  pipe(right_pipe);
  /* Fork a child process */
  int pid = fork();
  if (pid == 0) {
    child_process(right_pipe);
  } else {
    close(right_pipe[READ_PIPE]);
    /* Write to the right pipe */
    int read_num;
    while (read(left_pipe[READ_PIPE], &read_num, sizeof(int)) > 0) {
      if (read_num % prime != 0) {
        write(right_pipe[WRITE_PIPE], &read_num, sizeof(int));
      }
    }

    /* Close all the pipes*/
    close(left_pipe[READ_PIPE]);
    close(right_pipe[WRITE_PIPE]);
    int st;
    wait(&st);
    exit(0);
  }
}

int main(int argc, char* args[]) {
  int left_pipe[2];
  pipe(left_pipe);
  int pid = fork();
  if (pid == 0) {
    child_process(left_pipe);
  } else {
    close(left_pipe[READ_PIPE]);

    /* Write to the left pipe */
    for (int i = 2; i <= 35; i++) {
      write(left_pipe[WRITE_PIPE], &i, sizeof(int));
    }
    /* Close all the pipes*/
    close(left_pipe[WRITE_PIPE]);
    int st;
    wait(&st);
  }
  return 0;
}