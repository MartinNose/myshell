#ifndef MYSHELL_ENV_H
#define MYSHELL_ENV_H

#include <iostream>
#include <unistd.h>
#include <vector>
#include <map>
#include <fstream>
#include <sys/ioctl.h>
#include <cstdio>
#include <filesystem>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

using namespace std;
namespace fs = std::filesystem;

extern char **environ;

enum STATUS {
    NORMAL, INT, HUP, QUIT, ERR
};

struct BuiltIn {
    int echo(const string& argc, const vector<string> argv);
    int bg(const string& argc, const vector<string> argv);
    int cd(const string& argc, const vector<string> argv);
    int clr(const string& argc, const vector<string> argv);
    int dir(const string& argc, const vector<string> argv);
    int exec(const string& argc, const vector<string> argv);
    int exit(const string& argc, const vector<string> argv);
    int environ_func(const string& argc, const vector<string> argv);
    int fg(const string& argc, const vector<string> argv);
    int help(const string& argc, const vector<string> argv);
    int jobs(const string& argc, const vector<string> argv);
    int pwd(const string& argc, const vector<string> argv);
    int quit(const string& argc, const vector<string> argv);
    int set(const string& argc, const vector<string> argv);
    int shift(const string& argc, const vector<string> argv);
    int test(const string& argc, const vector<string> argv);
    int time(const string& argc, const vector<string> argv);
    int umask_func(const string& argc, const vector<string> argv);
    int unset(const string& argc, const vector<string> argv);
};

typedef int (BuiltIn::*builtin_func)(const string &, const vector<string>);
typedef map<string, builtin_func> map_builtin;


class ENV {
public:
    ENV();
    ~ENV() = default;

    //string CWD;
    string PS1;
    string getCWD();
    string parent;
    string SHELL;
    map<string, string> vars;
    map_builtin builtins;
    string file_descriptor1;
    string file_descriptor2;
    int cols;
    int rows;
    int stat;
};

#endif
