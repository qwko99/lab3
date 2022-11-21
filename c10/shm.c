#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SHMSIZE 1024
#define SHMKEY (key_t)0111
#define SEMKEY (key_t)0111

static int semid;

union semapore
{
    int value;
    struct semid_ds *buf;
    unsigned short int *array;
};

static int semid;

// 부모 프로세스 (파일 읽기 역할만 수행)
int parent_process_read(char *filename)
{
    int shmid, len;
    void *shmaddr;

    FILE *fp = fopen(filename, "r");
    // 파일읽기

    int size = strlen((char *)shmaddr);

    printf("부모 프로세스(읽기) 시작!\n");
    sleep(3);

    if ((shmid = shmget(SHMKEY, SHMSIZE, IPC_CREAT | 0666)) == -1)
    {
        perror("shmget failed");
        exit(1);
    }
    //공유 메모리를 생성하거나 접근하기 위한 함수

    if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }
    //공유 메모리를 프로세스에서 사용할 수 있게 하는 함수

    fgets((char *)shmaddr, size, fp);
    //파일로 부터 문자열을 가지고 오는 함수

    if (shmdt(shmaddr) == -1)
    {
        perror("shmdt failed");
        exit(1);
    }
    //공유 메모리를 프로세스에서 분리하기 위한 함수

    printf("읽기 성공\n");
    fclose(fp);
    printf("부모 프로세스 종료.\n\n");
}

// 자식 프로세스 (파일 쓰기 역할만 수행)
int child_process_write(char *filename)
{
    int shmid, len;
    void *shmaddr;
    FILE *fp = fopen(filename, "w");

    printf("자식 프로세스(쓰기) 시작!\n");
    sleep(2);

    if ((shmid = shmget(SHMKEY, SHMSIZE, IPC_CREAT | 0666)) == -1)
    {
        perror("shmget failed");
        exit(1);
    }
    //공유 메모리를 생성하거나 접근하기 위한 함수

    if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }
    //공유 메모리를 프로세스에서 사용할 수 있게 하는 함수

    fputs((char *)shmaddr, fp);
    //파일에 문자열을 쓰는 함수

    printf("복사 성공!\n");

    if (shmdt(shmaddr) == -1)
    {
        perror("shmdt failed");
        exit(1);
    }
    //공유 메모리를 프로세스에서 분리하기 위한 함수

    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        perror("shmctl failed");
        exit(1);
    }
    //공유 메모리를 제어하기 위한 함수

    fclose(fp);
    printf("자식 프로세스 종료.\n\n");
}

int main(int argc, char **argv)
{
    FILE *fp;
    union semapore semapore;

    struct sembuf mysem_open = {0, -1, SEM_UNDO};
    struct sembuf mysem_close = {0, 1, SEM_UNDO};

    if (argc != 3)
    {
        fprintf(stderr, "이렇게 적어주세요: %s <원본 파일 이름> <생성할 파일 이름>\n", argv[0]);
        exit(1);
    }

    semid = semget((key_t)234, 1, 0660 | IPC_CREAT);
    // semid라는 세마포어 식별자를 얻는데 쓰이는 시스템콜

    if (semid == -1)
    {
        perror("semget error");
        exit(0);
    }

    semapore.value = 1;

    if (semctl(semid, 0, SETVAL, semapore) == -1)
    {
        perror("semctl error");
        exit(1);
    }
    //세마포어를 제어할 수 있는 시스템 콜

    if (semop(semid, &mysem_open, 1) == -1)
    {
        perror("semop error");
        exit(0);
    }
    //세마포어의 값을 증가, 감소시킴으로써 크리티컬 섹션 전 후에 사용

    if ((fp = fopen(argv[1], "r+")) == NULL)
    {
        perror("fopen error");
        exit(0);
    }

    parent_process_read(argv[1]);
    rewind(fp);

    sleep(2);

    child_process_write(argv[2]);
    fclose(fp);

    semop(semid, &mysem_close, 1);

    printf("만들어진 %s 파일을 확인하세요.\n", argv[2]);

    return 0;
}