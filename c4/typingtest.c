#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#define PASSWORDSIZE 12
int main(void)
{
    time_t start_time, end_time, total_time;
    int fd;
    int nread, cnt = 0, errcnt = 0;
    char ch, text[] = "The magic thing is that you can change it.";
    struct termios init_attr,
        new_attr;
    fd = open(ttyname(fileno(stdin)), O_RDWR);
    tcgetattr(fd, &init_attr);
    new_attr = init_attr;
    new_attr.c_lflag &= ~ICANON;
    new_attr.c_lflag &= ~ECHO;
    /* ~(ECHO | ECHOE | ECHOK |
    ECHONL); */
    new_attr.c_cc[VMIN] = 1;
    new_attr.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &new_attr) != 0)
    {
        fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
    }
    start_time = time(NULL);
    printf("다음 문장을 그대로 입력하세요.\n%s\n", text);
    while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n')
    {
        if (ch == text[cnt++])
            write(fd, &ch, 1);
        else
        {
            write(fd, "*", 1);
            errcnt++;
        }
    }
    end_time = time(NULL);
    total_time = end_time - start_time;
    int total = (strlen(text)) / total_time * 60;
    printf("\n소요시간 약%d초\n", total_time);
    printf("평균 분당 타자수: %ld\n", total);
    printf("타이핑 오류의 횟수는 %d\n", errcnt);
    tcsetattr(fd, TCSANOW, &init_attr);
    close(fd);
}
