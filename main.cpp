#include <iostream>
#include "interpreter.h"
#include <csignal>
#include <filesystem>
#include <unistd.h>
pid_t child_pid;

void SIGHandler(int sig) {
    if (sig == SIGINT) {
        exit(130);
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, SIGHandler);
    interpreter interpreter;

    if (argc > 1) { // inside
        string arg = argv[1];
        //string set = "parent=shell";
        //putenv((char *)set.c_str());
        for (int i = 2; i < argc; i++) {
            arg += ' ';
            arg += argv[i];
        }
        interpreter.readline(arg);
    }

    interpreter.main_loop();

    return 0;
}
