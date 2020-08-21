#ifndef MYSHELL_INTERPRETER_H
#define MYSHELL_INTERPRETER_H

#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "interpreter.h"
#include "ENV.h"

#define INPUT_END 1
#define OUTPUT_END 0  

using namespace std;

class interpreter {
public:
    interpreter() = default;
    void main_loop ();
    string get_time();
    int readline(const string &line);
    int process_external(const string &inputline);
private:
    ENV env;
    int expand(string &line);
};


#endif //MYSHELL_INTERPRETER_H
