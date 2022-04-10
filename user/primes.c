#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define INT_SIZE sizeof(int)

void primes(int prime_pipe[2]) {
    close(prime_pipe[1]);
    int prime_num, other_num;

    if(read(prime_pipe[0], &prime_num, INT_SIZE) != INT_SIZE) {
        fprintf(2, "fail to read from pipe.\n");
        exit(1);
    }
    printf("prime %d\n", prime_num);
    
    if(read(prime_pipe[0], &other_num, INT_SIZE) == INT_SIZE) {
        int next_pipe[2];
        int pid;
        pipe(next_pipe);

        switch (pid = fork())
        {
        case -1:
            fprintf(2, "fork failed.\n");
            exit(1);
        case 0:
            primes(next_pipe);
            exit(0);
        default:
            close(next_pipe[0]);
            if(other_num % prime_num) {
                if(write(next_pipe[1], &other_num, INT_SIZE) != INT_SIZE) {
                    fprintf(2, "fail to write to pipe");
                    exit(1);
                }
            }

            while(read(prime_pipe[0], &other_num, INT_SIZE) > 0) {
                if(other_num % prime_num) {
                    if(write(next_pipe[1], &other_num, INT_SIZE) < 0) {
                        fprintf(2, "fail to write to pipe");
                        exit(1);
                    }
                }
            }
            close(next_pipe[1]);
        }
    }
    close(prime_pipe[0]);
    wait(0);
    exit(0);
}

int main(int argc, char *argv[]) {
    int prime_pipe[2];
    int pid;
    pipe(prime_pipe);

    switch (pid = fork())
    {
    case -1:
        fprintf(2, "fork failed.\n");
        exit(1);
    case 0:
        primes(prime_pipe);
        exit(0);
    default:
        close(prime_pipe[0]);
        for(int i = 2; i <= 35; i++) {
            if(write(prime_pipe[1], &i, INT_SIZE) != INT_SIZE) {
                fprintf(2, "write to pipe failed.\n");
                exit(1);
            }
        }
        close(prime_pipe[1]);
        wait(0);
        exit(0);
    }
    return 0;
}