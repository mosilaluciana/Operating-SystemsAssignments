#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define FIFO_WRITE "RESP_PIPE_35801"
#define FIFO_READ "REQ_PIPE_35801"


int main()
{
    if(mkfifo(FIFO_WRITE,0600) != 0)
    {
        printf("ERROR\ncannot create the response pipe\n");
        return 1;
    }

    int fd_read = -1;
    int fd_write = -1;

    fd_read = open(FIFO_READ,O_RDONLY);
    if(fd_read == -1)
    {
        printf("ERROR\ncannot open the request pipe\n");
        unlink(FIFO_WRITE);
        return 1;
    }

    fd_write = open(FIFO_WRITE,O_WRONLY);

    if(fd_write == -1)
    {
        printf("ERROR\ncannot open the response pipe\n");
        close(fd_read);
        unlink(FIFO_WRITE);
        return 1;
    }
    int dim=5;

    if(write(fd_write,&dim,1) != 1)
    {
        printf("ERROR\n");
        return 1;
    }
    if(write(fd_write,"BEGIN",5) != 5)
    {
        printf("ERROR\n");
        return 1;
    }
    printf("SUCCESS\n");
    }
