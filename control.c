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

#define SEM_KEY 23
#define SHM_KEY 32

int main(int argc, char *argv[]) {
  int md, val, fd, shmd;
  char story[250];

  if (argc > 1) {
    if (!strcmp(argv[1], "-c")) {
      md = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0664);

      if (md == -1) {
        printf("semaphore already exists\n");
        return 0;
      }
      printf("semaphore created: %d\n", md);

      semctl(md, 0, SETVAL, 1);
      val = semctl(md, 0, GETVAL);
      printf("value set: %d\n", val);

      shmd = shmget(SHM_KEY, 32, IPC_CREAT | IPC_EXCL);

      fd = open("story", O_CREAT | O_TRUNC, 0666);
      close(fd);
    }
    else if (!strcmp(argv[1], "-v")) {
      fd = open("story", O_RDONLY);
      read(fd, &story, sizeof(story));
      printf("Story:\n\n%s\n\n", story);
      close(fd);
    }
    else if (!strcmp(argv[1], "-r")) {
      fd = open("story", O_RDONLY);
      read(fd, &story, sizeof(story));
      printf("Story:\n\n%s\n\n", story);
      close(fd);

      shmd = shmget(SHM_KEY, 32, 0);
      shmctl(shmd, IPC_RMID, 0);

      md = semget(SEM_KEY, 1, 0664);
      val = semctl(md, 0, IPC_RMID);
      printf("semaphore removed: %d\n", val);
      execlp("rm", "story", (char *) NULL);
    }
  }
  else {
    printf("Incorrect arguments\n");
  }
  return 0;
}
