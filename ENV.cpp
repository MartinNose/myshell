#include "ENV.h"

ENV::ENV() {
    //CWD = getCWD();
    PS1 = " $ ";
    SHELL = getCWD();
    file_descriptor1 = SHELL + "/" + ".input_buff";
    file_descriptor2 = SHELL + "/" + ".output_buff";
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    rows = w.ws_row;
    cols = w.ws_col;
    stat = NORMAL;

    builtins["echo"] = &BuiltIn::echo;
    builtins["cd"] = &BuiltIn::cd;
    builtins["clr"] = &BuiltIn:: clr;
    builtins["dir"] = &BuiltIn::dir;
    builtins["echo"] = &BuiltIn::echo;
    builtins["exec"] = &BuiltIn::exec;
    builtins["exit"] = &BuiltIn::exit;
    builtins["environ"] = &BuiltIn::environ_func;
    builtins["help"] = &BuiltIn::help;
    builtins["jobs"] = &BuiltIn::jobs;
    builtins["pwd"] = &BuiltIn::pwd;
    builtins["quit"] = &BuiltIn::quit;
    builtins["set"] = &BuiltIn::set;
    builtins["shift"] = &BuiltIn::shift;
    builtins["test"] = &BuiltIn::test;
    builtins["time"] = &BuiltIn::time;
    builtins["umask"] = &BuiltIn::umask_func;
    builtins["unset"] = &BuiltIn::unset;

    //TODO init vars
}

int BuiltIn::help(const string &argc, const vector<string> argv) {

}

int BuiltIn::jobs(const string &argc, const vector<string> argv) {

}

std::string ENV::getCWD() {
    char buff[100];
    getcwd(buff, 100);
    std::string cwd(buff);
    return cwd;
}

std::string getCWD() {
    char buff[100];
    getcwd(buff, 100);
    std::string cwd(buff);
    return cwd;
}

int BuiltIn::echo(const string &argc, const vector<string> argv) {
    cout << argv[1];
    for (auto i = argv.begin() + 2; i < argv.end(); i++) {
        cout << " " << *i;
    }
    cout << "\n";
    return 0;
}

int BuiltIn::bg(const string &argc, const vector<string> argv) {

}

int BuiltIn::fg(const string &argc, const vector<string> argv) {

}

int BuiltIn::cd(const string &argc, const vector<string> argv) {
    string path;
    if (argv.size() == 1) {
        path = string(getenv("HOME"));
    }
    else {
        path = argv[1];
    }

    setenv("PWD", (char *)path.c_str(), 1);
    return chdir((char *)path.c_str());
}

int BuiltIn::clr(const string &argc, const vector<string> argv) {
    if(system("CLS")) system("clear"); // avoided evil process
}

int BuiltIn::dir(const string &argc, const vector<string> argv) {
    string path;
    //cout << argv.size() << "calling dir\n";
    if (argv.size() == 2) path = argv[1];
    else path = getCWD();
    for (const auto & entry : fs::directory_iterator(path))
        std::cout << entry.path() << std::endl;
}

int BuiltIn::exec(const string &argc, const vector<string> argv) {
    string tmp;
    for (auto &i : argv) {
        tmp += i;
        tmp += " ";
    }
    tmp.pop_back();
    return system((char *)tmp.c_str());
}

int BuiltIn::exit(const string &argc, const vector<string> argv) {
    std::exit(stoi(argv[1]));
}

int BuiltIn::environ_func(const string &argc, const vector<string> argv) {
    char *s = *environ;
    cout << s << "\n";
    for (int i = 1; s; i++) {
        cout << s << "\n";
        s=*(environ + i);
    }
    return 0;
}

int BuiltIn::pwd(const string &argc, const vector<string> argv) {
    cout << getCWD() << "\n";
}

int BuiltIn::quit(const string &argc, const vector<string> argv) {
    std::exit(0);
}

int BuiltIn::set(const string &argc, const vector<string> argv) {
    environ_func(argc, argv);
}

int BuiltIn::shift(const string &argc, const vector<string> argv) {
    string tmp;
    for (auto &i : argv) {
        tmp += i;
        tmp += " ";
    }
    tmp.pop_back();
    return system((char *)tmp.c_str());
}

int BuiltIn::test(const string &argc, const vector<string> argv) {
    string tmp;
    for (auto &i : argv) {
        tmp += i;
        tmp += " ";
    }
    tmp.pop_back();
    return system((char *)tmp.c_str());
}

int BuiltIn::time(const string &argc, const vector<string> argv) {
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    cout << std::ctime(&end_time);
}

int BuiltIn::umask_func(const string &argc, const vector<string> argv) {
    if (argv.size() == 1) {
        mode_t oldmask;
        cout << umask(S_IRWXG);
    } else {
        umask(stoi(argv[1]));
    }
}

int BuiltIn::unset(const string &argc, const vector<string> argv) {
    if (argv.size() == 1) { 
        cout << "unset: not enough arguments. \n";
        return 1;
    }
    unsetenv((char *)argv[1].c_str());
}
