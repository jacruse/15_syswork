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

int main(int argc, char ** argv) {
  int sd, val, fd, shmd;
  union semun value_passer;
  char story[1024];
  
  if (argc < 2) {
    printf("Too few args...\nTry again next time\n");
    printf("Maybe try -c N, -v, or -r\n");
    exit(0);
  }

  if (!strcmp(argv[1], "-c")) {
    
    if ((sd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644)) == -1) {
      if (!(strcmp(strerror(errno), "File exists"))) {
	printf("semaphore already exists\n");
	exit(0);
      }
      printf("%s\n", strerror(errno));
      exit(0);
    }

    printf("semaphore created: %d\n", sd);

    value_passer.val = 1;
    
    if ((semctl(sd, 0, SETVAL, value_passer)) == -1) {
       printf("%s\n", strerror(errno));
      exit(0);
    }

    if ((val = semctl(sd, 0, GETVAL)) == -1) {
      printf("%s\n", strerror(errno));
      exit(0);
    }
    printf("value set to: %d\n", val);

    if ((shmd = shmget(SHM_KEY, 32, IPC_CREAT | IPC_EXCL | 0644)) == -1) {
      printf("%s\n", strerror(errno));
      exit(0);
    }

    if ((fd = open("story", O_CREAT | O_TRUNC, 0666)) == -1) {
      printf("%s\n", strerror(errno));
    }

    if (close(fd) == -1) {
      printf("%s\n", strerror(errno));
    }     
  }

  else if (!strcmp(argv[1], "-v")) {
    if ((fd = open("story", O_RDONLY)) == -1) {
      printf("%s\n", strerror(errno));
    }
    
    if (read(fd, &story, sizeof(story)) == -1) {
      printf("%s\n", strerror(errno));
    }
    
    printf("Story:\n\n%s\n\n", story);

    if (close(fd) == -1) {
      printf("%s\n", strerror(errno));
    }
  }

  else if (!strcmp(argv[1], "-r")) {
    if ((fd = open("story", O_RDONLY)) == -1) {
      printf("%s\n", strerror(errno));
    }
    
    if (read(fd, &story, sizeof(story)) == -1) {
      printf("%s\n", strerror(errno));
    }
    
    printf("Story:\n\n%s\n\n", story);

    if (close(fd) == -1) {
      printf("%s\n", strerror(errno));
    }
    
    if ((shmd = shmget(SHM_KEY, 32, 0)) == -1) {
      printf("%s\n", strerror(errno));
      exit(0);
    }

    if (shmctl(shmd, IPC_RMID, 0) == -1) {
      printf("%s\n", strerror(errno));
      exit(0);
    }

    printf("shared memory removed\n");

    if ((sd = semget(SEM_KEY, 1, 0)) == -1) {
      printf("%s\n", strerror(errno));
      exit(0);
    }

    if ((semctl(sd, 0, IPC_RMID)) == -1) {
      printf("%s\n", strerror(errno));
      exit(0);
    }

    printf("semaphore removed\n");

    remove("story");

    printf("story removed\n");
  }

  else {
    printf("HEY! Give me something to work with...\n");
    printf("Maybe try -c N, -v, or -r\n");
    exit(0);
  }

}

