#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
/**
 * Implement the UNIX program sleep for xv6; your sleep should pause for a
 * user-specified number of ticks. A tick is a notion of time defined by the xv6
 * kernel, namely the time between two interrupts from the timer chip. Your
 * solution should be in the file user/sleep.c.
 */

int main(int argc, char **argv) {
  /* Check the arguments */
  if (argc < 2) {
    fprintf(2, "Usage: sleep ticks \n");
    exit(1);
  }

  /* Call sleep() */
  int ticks = atoi(argv[1]);
  int status = sleep(ticks);
  if (status != 0) {
    fprintf(2, "Error when calling sleep");
    exit(1);
  }

  exit(0);
}