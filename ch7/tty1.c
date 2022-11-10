#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(void)
{
    int infd, outfd;
    int nread;
    char buf[BUFSIZE];
    printf("fd 0: ttyname 은 %s\n", isatty(0) ? ttyname(0) : "tty가 아님");
    printf("fd 1: ttyname 은 %s\n", isatty(1) ? ttyname(1) : "tty가 아님");
    printf("fd 2: ttyname 은 %s\n", isatty(2) ? ttyname(2) : "tty가 아님");
    infd = open("/dev/tty", O_RDONLY);
    outfd = open("/dev/tty", O_WRONLY);
    if (!infd || !outfd)
    {
        fprintf(stderr, "/dev/tty 를 열 수 없음\n");
        exit(1);
    }
    while ((nread = read(infd, buf, BUFSIZE)) > 0)
    {
        buf[nread] = '\0';
        write(outfd, buf, nread);
    }
    write(outfd, "/dev/tty 를 닫음\n", 20);
    close(infd);
    close(outfd);
}
