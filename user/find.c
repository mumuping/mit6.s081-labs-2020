#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

/**
 * Write a simple version of the UNIX find program: find all the files in a
 * directory tree with a specific name. Your solution should be in the file
 * user/find.c.
 */

/**
 * @brief Find all files in a directory tree with a specific name.
 * @param path The path to search
 * @param name The name to search for
 */
int find_files(char *path, char *name) {
  int fd = open(path, 0);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return -1;
  }

  /* Read the directory entries */
  struct stat st;
  struct dirent de;
  char buf[512] = {0};
  strcpy(buf, path);
  char *start = buf + strlen(buf);
  char *p;
  while (read(fd, &de, sizeof(de)) == sizeof(de)) {
    if (de.inum == 0 || strcmp(de.name, ".") == 0 ||
        strcmp(de.name, "..") == 0) {
      continue;
    }
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path <%s> too long\n", path);
      return -1;
    }
    /* Construct the next path name */
    p = start;
    *p++ = '/';
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    /* Get the file status */
    if (stat(buf, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", buf);
      return -1;
    }
    /* Print the file name if it matches the search name */
    if (st.type == T_FILE && strcmp(de.name, name) == 0) {
      printf("%s\n", buf);
    }
    /* Recursively search for files in the directory */
    else if (st.type == T_DIR) {
      if (find_files(buf, name) != 0) {
        return -1;
      }
    }
  }
  close(fd);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(2, "Usage: find <path> <name>\n");
    exit(1);
  }
  find_files(argv[1], argv[2]);
  exit(0);
}