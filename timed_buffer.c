#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>

#ifndef max
#define max(a,b) (((a) (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


#define BLOCK_SIZE 512
#define BUFFER_SIZE 64*1024*1024
int sig = 0;
struct timeval starttime;

FILE *fp;

void sig_handler(int signo)
{
    if (signo == SIGUSR1) {
        printf("received SIGUSR1\n");
        gettimeofday(&starttime, NULL);
        fp = fopen( strcat(strtok(ctime((const time_t *) &starttime.tv_sec), "\n") , ".ts"), "w" );
        sig = 1;
    } else if (signo == SIGKILL)
        printf("received SIGKILL\n");
    else if (signo == SIGSTOP)
        printf("received SIGSTOP\n");
}


int main(int argc, const char *argv[])
{

  //printf("Debug");

  int delay = 0;
  if (argc != 1) {
    delay  = atoi( argv[1]);
  }

  int output = 0; //to be set to 1 when delay is reached

  uint8_t *buffer = malloc(BUFFER_SIZE);
  uint8_t *write = buffer;
  uint8_t *read = buffer;
  int n_read = BLOCK_SIZE;
  int n_write = 0;

  struct timeval tv;
  gettimeofday(&tv, NULL);

  struct timeval tv2;


  while (1) {
    if (signal(SIGUSR1, sig_handler) == SIG_ERR)
      printf("\ncan't catch SIGUSR1\n");
    if (!output) {
      gettimeofday(&tv2, NULL);
      if (tv2.tv_sec - tv.tv_sec >= delay) {
        output = 1;
        fprintf(stderr, "Required buffer size: %iKb\n", (int) (write - read) / 1024);
      }
    }

    n_read = min(BLOCK_SIZE, BUFFER_SIZE - (write - buffer));

    // printf("Debug %i %ui %ui %ui\n",n_read, write, buffer, read );

    size_t bytes = fread(write, sizeof(uint8_t), n_read, stdin);

    //printf("Debug");
    if (bytes == 0) {
      sleep(1);
    } else {

      write = write + bytes;
      if (write >= buffer + BUFFER_SIZE) {
        if (!output) {
          fprintf(stderr, "Buffer overflow\n");
          exit(1);
        } else {
          write = buffer;
        }
      }
      if (output && sig) {
        struct timeval stoptime;
        gettimeofday(&stoptime, NULL);
        if(stoptime.tv_sec - starttime.tv_sec >= delay) {
          sig = 0;
          fclose(fp);
          buffer = read;
        }

        n_write = min(bytes, buffer + BUFFER_SIZE - read);
        fwrite(read, sizeof(uint8_t), n_write, fp);
        read += n_write;
        if (bytes > n_write) {
          n_write = bytes - n_write;
          read = buffer;
          fwrite(read, sizeof(uint8_t), n_write, fp);
          read += n_write;
        }
      }
    }
  }
  return 0;
}

