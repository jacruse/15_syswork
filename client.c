#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <errno.h>

#define SEM_KEY 23
#define SHM_KEY 32
#define UP 1
#define DOWN -1

int main() {
  int sd, shmd, fd;
  struct sembuf operator;
  int * size;
  char input_line[64];

  if ((sd = semget(SEM_KEY, 1, 0)) == -1) {
      printf("%s\n", strerror(errno));
      exit(0);
  }

  operator.sem_num = 0;
  operator.sem_op = DOWN;
  operator.sem_flg = SEM_UNDO;

  if (semop(sd, &operator, 1)) {
    printf("%s\n", strerror(errno));
    exit(0);
  }

  if ((shmd = shmget(SHM_KEY, 32, 0)) == -1) {
    printf("%s\n", strerror(errno));
    exit(0);
  }

  if ((size = (int *) shmat(shmd, 0, 0)) == -1) {
    printf("%s\n", strerror(errno));
    exit(0);
  }

  if ((fd = open("story", O_APPEND | O_RDWR)) == -1) {
    printf("%s\n", strerror(errno));
    exit(0);
  }

  if (*size) {
    char * last_line = (char *) calloc(1, *size);
    lseek(fd, -1 * (*size), SEEK_END);
    read(fd, last_line, *size);
    printf("Last line entered:\n%s\n\n", last_line);
    free(last_line);
  }
  
  else {
    printf("No lines have been previously entered\n\n");
  }

  printf("Add a line to the story:\n");

  if (!fgets(input_line, sizeof(input_line), stdin)) {
    printf("%s\n", strerror(errno));
    exit(0);
  }

  *size = strlen(input_line);

  lseek(fd, 0, SEEK_END);

  write(fd, input_line, *size);

  shmdt(size);
  close(fd);
}
