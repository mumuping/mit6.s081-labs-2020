#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
/**
 * Write a program that uses UNIX system calls to ''ping-pong'' a byte between
 * two processes over a pair of pipes, one for each direction. The parent should
 * send a byte to the child; the child should print "<pid>: received ping",
 * where <pid> is its process ID, write the byte on the pipe to the parent, and
 * exit; the parent should read the byte from the child, print "<pid>: received
 * pong", and exit. Your solution should be in the file user/pingpong.c.
 */

int main(int argc, char* argv[]) {
  /* Define the pipes */
  int parent_to_child[2];
  int child_to_parent[2];

  pipe(parent_to_child);
  pipe(child_to_parent);

  /* Fork */
  int pid = fork();
  if (pid == 0) {
    /* Close the write end of the parent_to_child pipe in child */
    close(parent_to_child[1]);
    /* Close the read end of the child_to_parent pipe in child */
    close(child_to_parent[0]);

    char read_buf;
    read(parent_to_child[0], &read_buf, 1);
    /* Close the read end of the parent_to_child pipe in child */
    close(parent_to_child[0]);

    printf("%d: received ping\n", getpid());

    /* Write to the parent */
    write(child_to_parent[1], &read_buf, 1);
    /* Close the write end of the child_to_parent pipe in child */
    close(child_to_parent[1]);
  } else {
    close(parent_to_child[0]);
    close(child_to_parent[1]);

    char write_buf;
    write(parent_to_child[1], &write_buf, 1);
    close(parent_to_child[1]);

    char read_buf;
    read(child_to_parent[0], &read_buf, 1);
    close(child_to_parent[0]);

    printf("%d: received pong\n", getpid());

    /* wait for child process */
    int state;
    wait(&state);
  }

  exit(0);
}