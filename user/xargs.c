#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// readlines reads user input(a long string) from stdin, seperate arguments
// from the string and store them in array `argv`, starting from `edge`, and
// return the value of `edge` after reading arguments.
int readlines(int edge, char *argv[]) {
    char buf[1024];
    int n = 0;

    // read arguments from stdin until hitting a newline.
    // The argument string must not longer than 1023 bytes.
    while(read(0, buf + n, 1)) {
        if(n == 1023) {
            fprintf(2, "argument list too long\n");
            exit(1);
        }
        if(buf[n] == '\n') {
            break;
        }
        n++;
    }
    buf[n] = 0;

    // if got nothing from stdin, simply return.
    if(n == 0) {
        return 0;
    }

    int offset = 0;
    while(offset < n) {
        argv[edge++] = buf + offset;
        while(buf[offset] != ' ' && offset < n) {
            offset++;
        }
        while(buf[offset] == ' ' && offset < n) {
            buf[offset++] = 0;
        }
    }
    return edge;
}


int main(int argc, char *argv[]) {
    if(argc <= 1) {
        fprintf(2, "usage: xargs command [arguments]\n");
        exit(1);
    }

    // extract the command from argument list.
    char *command = malloc(strlen(argv[1]) + 1);
    strcpy(command, argv[1]);

    // allocate a new array to store arguments from both argument list
    // and extracted from stdin.
    char *new_args[MAXARG];
    for(int i = 1; i < argc; i++) {
        new_args[i-1] = malloc(strlen(argv[i]) + 1);
        strcpy(new_args[i-1], argv[i]);
    }

    int cur_argc = 0;
    while((cur_argc = readlines(argc - 1, new_args)) != 0) {
        new_args[cur_argc] = 0;
        if(fork() == 0) {
            exec(command, new_args);
            fprintf(2, "exec failed.\n");
            exit(1);
        }
        wait(0);
    }

    exit(0);
}