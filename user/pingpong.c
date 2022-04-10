#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int ping[2], pong[2];
    pipe(ping);
    pipe(pong);

    int pid;
    switch (pid = fork())
    {
    case -1:
        fprintf(2, "fork failed.\n");
        break;
    case 0:
        close(ping[1]);
        close(pong[0]);

        char buf_c[1];
        if(read(ping[0], buf_c, sizeof(buf_c)) > 0) {
            printf("%d: received ping\n", getpid());
            if(write(pong[1], buf_c, sizeof(buf_c)) > 0) {
                exit(0);
            }
            exit(1);
        }
        exit(1);
        break;
    default:
        close(ping[0]);
        close(pong[1]);

        char buf_p[1] = {'1'};
        if(write(ping[1], buf_p, sizeof(buf_p)) > 0) {
            if(read(pong[0], buf_p, sizeof(buf_p)) > 0) {
                printf("%d: received pong\n", getpid());
                exit(0);
            }
            exit(1);
        }
        exit(1);
    }

    exit(0);
}