#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "common.h"
#include "parse.h"

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
  if (fd < 0) {
    printf("Got a bad file descriptor from user\n");
    return STATUS_ERROR;
  }

  int realcount = dbhdr->count;

  dbhdr->magic = htonl(dbhdr->magic);
  dbhdr->filesize = htonl(dbhdr->filesize);
  dbhdr->count = htons(dbhdr->count);
  dbhdr->version = htons(dbhdr->version);

  lseek(fd, 0, SEEK_SET);

  write(fd, dbhdr, sizeof(struct dbheader_t));

  for(int i = 0; i < realcount; i++) {
    employees[i].hours = htonl(employees[i].hours);
    write(fd, &employees[i], sizeof(struct employee_t));
  }

  return STATUS_SUCCESS;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd < 0) {
    printf("Got a bad file descriptor from user\n");
    return STATUS_ERROR;
  }

  struct dbheader_t *dbhdr = calloc(1, sizeof(struct dbheader_t));
  if(dbhdr == NULL) {
    perror("calloc");
    return STATUS_ERROR;
  }

  if (read(fd, dbhdr, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
    perror("read");
    free(dbhdr);
    return STATUS_ERROR;
  }

  // unpack the header
  dbhdr->version = ntohs(dbhdr->version);
  dbhdr->count = ntohs(dbhdr->count);
  dbhdr->magic = ntohl(dbhdr->magic);
  dbhdr->filesize = ntohl(dbhdr->filesize);

  // check version
  if (dbhdr->version != 1) {
    printf("Improper header version\n");
    free(dbhdr);
    return STATUS_ERROR;
  }

  // check magic number
  if (dbhdr->magic != HEADER_MAGIC) {
    printf("Improper header magic number\n");
    free(dbhdr);
    return STATUS_ERROR;
  }

  // check filesize
  struct stat dbstat = {0};
  if (fstat(fd, &dbstat) == -1) {
    perror("fstat");
    free(dbhdr);
    return STATUS_ERROR;
  }
  if (dbstat.st_size != dbhdr->filesize) {
    printf("Corrupted database\n");
    free(dbhdr);
    return STATUS_ERROR;
  }

  *headerOut = dbhdr;
}

int create_db_header(struct dbheader_t **headerOut) {
  if(headerOut == NULL) {
    printf("Got a null pointer from user\n");
    return STATUS_ERROR;
  }

	// create a new header
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if(header == NULL) {
    perror("calloc");
    return STATUS_ERROR;
  }

  header->version = 0x1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;

  return STATUS_SUCCESS;
}


