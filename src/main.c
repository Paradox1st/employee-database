#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n - create new database file\n");
  printf("\t -f <database file> - [required] path to database file\n");
  printf("\t -h - display this help message\n");
  return;
}

int main(int argc, char *argv[]) { 

  char *filepath = NULL;
  bool newfile = false;
	int c;

  while ((c = getopt(argc, argv, "nf:h")) != -1) {
    switch (c) {
      case 'n':
        newfile = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case 'h':
        print_usage(argv);
        return 0;
      case '?':
        fprintf(stderr, "Unknown option: %c\n", c);
        return 1;
      default:
        fprintf(stderr, "Unexpected error with option parsing.\n");
        return -1;
    }
  }

  if (filepath == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);

    return 0;
  }

  return 0;
}
