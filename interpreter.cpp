#include "interpreter.h"

vector<string> split(const string &line, const string &split=" \t") {
    vector<string> argv;
    char *p;
    char tmp[255];
    strcpy(tmp, line.c_str());
    p=strtok(tmp, (char *)split.c_str());
    while(p != nullptr) {
        argv.push_back(p);
        p=strtok(nullptr, (char *)split.c_str());
    }
    return argv;
}

int interpreter::expand(string &line) {
    if (line.find('$') == string::npos) return 0;

    vector<string> argv = split(line);
    for (auto &arg: argv) {
        if (arg[0] != '$') continue;
        arg.erase(0,1);    
        char tmp[255] = {};
        string t_str;
        if (arg.find('/') != string::npos) {
            strcpy(tmp, (char *)arg.c_str());
            char *p = strtok(tmp, "/");
            arg = string(p);
            t_str = string(strtok(nullptr, ""));
        }
        char *buff = getenv((char *)arg.c_str());
        arg.clear();
        if (buff != nullptr) {
            arg = string(buff);
        }
        if (!t_str.empty()) {
            arg += "/";
            arg += t_str;
        }
    }

    line = argv[0];
    for (auto ptr = argv.begin() + 1; ptr < argv.end(); ptr++) {
        line += " ";
        line += *ptr;
    }
    return 0;
}

int copy(const string path1, const string path2) {
    
    std::ifstream in{path1};
    std::remove((char *)path2.c_str());
    std::ofstream out{path2};
    if (!out) {
        std::cerr << "Could not create output file " << path2 << '\n';
        return 1;
    }
    static constexpr std::size_t buffsize{1024};
    char buffer[buffsize];
    while (in.read(buffer, buffsize)) {
        out.write(buffer, buffsize);
    }
    out.write(buffer, in.gcount());
}


int interpreter::readline(const string &inputline) {
    string line = inputline;
    expand(line);
    if (line.empty()) return 0;

    vector<string> argv;
    auto print = [](const string &arg) { cout << arg << "\n"; };
    if (line.find('=') != string::npos) {
        argv = split(line, "=");
        if (argv.size() == 2) {
            env.vars[argv[0]] = argv[1];
            setenv((char *)argv[0].c_str(), (char *)argv[1].c_str(), 1);
            return 0;
        } else {
            cerr << "Invalid assignment. Please check your syntax.\n";
            return ERR;
        }
    }

    if (line.find('|')  != string::npos) {
        argv = split(line, "|");
        setenv("isRedirect", "1", 1);

        if (argv.size() != 2) cout << "too many arguments\n";
        pid_t pid1;                                  
        pid_t pid2;                                 
        int fd[2];                                 
                                                
        pipe(fd);                                
        pid1 = fork();                              
        
        if(pid1==0) {                       
            close(fd[INPUT_END]);                  
            dup2(fd[OUTPUT_END], STDIN_FILENO);      
            close(fd[OUTPUT_END]);
            process_external(argv[0]);   
            exit(0);          
        }                                            
        else {                                            
            pid2=fork();                             
                                                    
            if(pid2==0) {                                     
                close(fd[OUTPUT_END]);               
                dup2(fd[INPUT_END], STDOUT_FILENO);  
                close(fd[INPUT_END]);                
                process_external(argv[1]); 
                exit(0); 
            }                                        
                                                    
            close(fd[OUTPUT_END]);                   
            close(fd[INPUT_END]);                    
            waitpid(-1, NULL, 0);                    
            waitpid(-1, NULL, 0);                                                       
        }          
        return 0;
    }
    if (line.find('<') != string::npos || line.find('>') != string::npos) {
        argv = split(line);
        int rdIn = -1;
        int rdOut = -1;
        for (auto i = argv.begin(); i < argv.end(); i++) {
            if (*i == "<" && i + 1 < argv.end()) {
                string tmp = *(i + 1);
                rdIn = open((char *)tmp.c_str(), O_RDWR);
                if (rdIn < 0) return ERR;
            } else if (*i == ">" && i + 1 < argv.end()) {
                string tmp = *(i + 1);
                rdOut = open((char *)tmp.c_str(), O_RDWR);
                if (rdOut < 0) return ERR;
            }
        }
        pid_t pid = fork();
        if (pid==0) {
            if (rdIn != -1) {
                dup2(rdIn, STDIN_FILENO);
                close(rdIn);
            }
            if (rdOut != -1) {
                dup2(rdOut, STDOUT_FILENO);
                close(rdOut);
            }
            vector<string> tmp = split(line, "<>");
            process_external(tmp[0]);
            exit(0);
        } else {
            wait(NULL);
            return 0;
        }
    }

    argv = split(line);

    if (argv[0]=="help") {
        readline("cat README.md | less");
        return 0;
    }

    auto func_ptr = env.builtins.find(argv[0]);

    if (func_ptr != env.builtins.end()) {
        BuiltIn b;
        vector<char*> c_argv;
        for (auto &i : argv) c_argv.push_back(&i[0]);
        c_argv.push_back(nullptr);

        return (b.*(func_ptr->second))(argv[0], argv);
    }

    // input is not a builtin function, calling external executables
    return process_external(line);

    cout << "myshell: command not found. \n";
    return ERR;
}

int interpreter::process_external(const string &inputline) {
    string line = inputline;
    vector<string> argv = split(line);
    pid_t child_id = fork();
    if (child_id == 0) {
        
        vector<char*> c_argv;
        for (auto &i: argv) c_argv.push_back(&i[0]);
        c_argv.push_back(nullptr);
        execvp(c_argv[0], c_argv.data());
        exit(0);
    } else {
        int st;
        wait(&st);
        
        return WEXITSTATUS(st);
    }
}

void errHandler();

void interpreter::main_loop() {
    const char *buff = getenv("isRedirect");
    if (buff == nullptr) cout << "Welcome to MYSHELL! Last login: " << get_time()  << endl;
    string input;
    while (env.stat != QUIT) {
        if (buff == nullptr) cout << env.getCWD() << env.PS1; // If show PS1
        getline(cin, input);

        env.stat = readline(input);

        string v = to_string(env.stat);
        setenv("?", (char *)v.c_str(), 1);

        if (errno) errHandler();
        switch (env.stat) {
            case QUIT:
                break;
            case NORMAL:
                break;
            case INT:
                cout << "^C\n";
                break;
            case HUP:
                cout << split(input)[0] << "\n";
                break;
        }
    }
}

std::string interpreter::get_time() {
    auto end = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    return std::ctime(&end_time);
}

void errHandler() {
    switch(errno) {
        case EACCES:
            cerr << "Search permission is denied for one of the components of path.\n";
            break;
        case EFAULT:
            cerr << "path points outside your accessible address space.\n";
            break;
        case EIO:
            cerr << "An I/O error occurred.\n";
            break;
        case ELOOP:
            cerr << "Too many symbolic links were encountered in resolving path.\n";
            break;
        case ENAMETOOLONG:
            cerr << "path is too long.        \n";
            break;
        case ENOENT:
            cerr << "The directory specified in path does not exist.\n";
            break;

        case ENOMEM:
            cerr << "Insufficient kernel memory was available.\n";
            break;
        case ENOTDIR:
            cerr << "A component of path is not a directory.\n";
            break;

        case EBADF:
            cerr << "fd is not a valid file descriptor.\n";
            break;
        default:
            break;
    }
    errno = 0;
}
