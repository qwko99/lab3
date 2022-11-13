#include <unistd.h>

int main(int argc, char *argv[])
{
    char *command[256];
    sprintf(command, "%s%s", "/bin/", argv[1]);
    execl(command, argv[1], argv[2], (char *)0);
    return 0;
}