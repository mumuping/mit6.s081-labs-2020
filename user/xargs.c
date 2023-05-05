#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"
/* Don't be too big */
#define MAX_PARAM_LENGTH 64

/**
 * Write a simple version of the UNIX xargs program: read lines from the
 * standard input and run a command for each line, supplying the line as
 * arguments to the command. Your solution should be in the file user/xargs.c.
 */

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "%s: missing command\n", argv[0]);
    exit(1);
  }
  if (argc - 1 > MAXARG) {
    fprintf(2, "%s: too many arguments\n", argv[1]);
    exit(1);
  }

  /* The params array */
  char params[MAXARG][MAX_PARAM_LENGTH];
  char *to_child_params[MAXARG + 1] = {0};
  /* Copy the original command line arguments to the params array */
  for (int i = 1; i < argc; i++) {
    if (strlen(argv[i]) >= MAX_PARAM_LENGTH) {
      fprintf(2, "%s: too long argument\n", argv[1]);
      exit(1);
    }
    strcpy(params[i - 1], argv[i]);
    to_child_params[i - 1] = params[i - 1];
  }
  /* The start of the params array that the stdin will be copied to */
  int start = argc - 1;
  /* Read lines from stdin */
  while (1) {
    char ch;
    char flag = 0;
    int offset = 0, param_start = start, read_status;
    memset(params[param_start], 0, (MAXARG - param_start) * MAX_PARAM_LENGTH);
    while ((read_status = read(0, &ch, 1)) > 0 && ch != '\n') {
      if (ch == ' ' || ch == '\t') {
        if (flag == 1) {
          if (param_start >= MAXARG) {
            fprintf(2, "%s: too many arguments\n", argv[1]);
            exit(1);
          }
          ++param_start;
          offset = 0;
          flag = 0;
        }
      } else {
        if (offset >= MAX_PARAM_LENGTH ||
            (offset == MAX_PARAM_LENGTH - 1 && ch != '\0')) {
          fprintf(2, "%s: too long argument\n", argv[1]);
          exit(1);
        }
        params[param_start][offset++] = ch;
        flag = 1;
      }
    }
    /* Read the EOF or something unexpected */
    if (read_status <= 0) {
      break;
    }

    for (int i = start; i <= param_start; i++) {
      to_child_params[i] = params[i];
    }

    /* Fork a child process to execute the command */
    int pid = fork();
    if (pid == 0) {
      exec(argv[1], to_child_params);
      exit(0);
    } else {
      wait(0);
    }
  }

  exit(0);
}