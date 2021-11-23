#include "ku_ps_input.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>

int ncount = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// int start, int end, int a1, int a2, int input[], int limit

// void *t_s(void *p){
//     printf("HIHI\n");
//     return 0;
// }

typedef struct
{
    int start;
    int end;
    int a1;
    int a2;
    int limit;
    int *arr;
} my_arg;

// typedef struct{

// }

void *t_search(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("Error1");
        return 0;
    }
    int range[3];
    for (int i = 0; i < 3; i++)
    {
        range[i] = atoi(argv[i + 1]);
    }
    if (range[0] > range[1])
    {
        printf("Error2");
        return 0;
    }
    else
    {

        char *filename = argv[4];
        int fd;
        int i = 0;
        char buff[5];
        int num = 0;
        int len = 0;
        void *thread_result;
        ssize_t rd_size;
        if ((fd = open(filename, O_RDONLY, 0644)) < 0)
        {
            fprintf(stderr, "open error for %s\n", filename);
            exit(1);
        }
        //정수 개수 받아오기위해서 따로 빼준 함수
        if (pread(fd, buff, 5, len) == -1)
        {
            perror("pread");
        };
        len += 6;
        int arrNum = atoi(buff);
        int NUMS[arrNum];
        //파일에 있는 숫자들만 다 읽기
        while (0 < (rd_size = pread(fd, buff, 5, len)))
        {
            NUMS[i] = atoi(buff);
            if (lseek(fd, len, SEEK_CUR) == -1)
            {
                printf("Error3");
            }
            i++;
            len += 6;
        };
        close(fd);

        //범위 나누기
        int arr[range[2] + 2];
        for (int i = 0; i < range[2] + 2; i++)
        {
            arr[i] = 0;
        }
        arr[range[2]] = arrNum;
        int thread_num = (arrNum / range[2]);
        //마지막 프로세스로 값이 몰리는 것을 방지하기 위한 코드
        if (thread_num == 0)
        {
            thread_num = 1;
        }
        for (int i = 1; i < range[2]; i++)
        {
            arr[i] = arr[i - 1] + thread_num;
        }
        my_arg arg;
        arg.a1 = range[0];
        arg.a2 = range[1];
        arg.limit = arrNum;
        arg.arr = NUMS;
        //쓰레드 생성
        pthread_t thread_id[range[2]];
        int status;

        // 쓰레드 생성할때 인자로 범위 넘겨주자! 범위 그 전에 만들어야한다!
        for (int i = 0; i < range[2]; i++)
        {
            arg.start = arr[i];
            arg.end = arr[i + 1];
            status = pthread_create(&thread_id[i], NULL, t_search, &arg);
            if (status != 0)
            {
                perror("create");
                exit(1);
            }
            status = pthread_join(thread_id[i], &thread_result);
            if (status != 0)
            {
                perror("pthread_join");
                exit(1);
            }
        }
        printf("%d\n", ncount);
    }
    return 0;
}

void *t_search(void *arg)
{
    my_arg *targ = (my_arg *)arg;
    int start = targ->start;
    int end = targ->end;
    int a1 = targ->a1;
    int a2 = targ->a2;
    int limit = targ->limit;
    for (int i = start; i < end; i++)
    {
        if (targ->arr[i] >= a1 && targ->arr[i] <= a2 && i < limit)
        {
            ncount++;
        }
    }
    return NULL;
}