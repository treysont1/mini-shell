# Mini Shell

A small UNIX-style shell written in C that supports commmand execution and multi-stage pipelines using fork(), execvp(), pipe(), and dup2().

This project started as a way to better understand how shells interact with the operating system, and learn about concepts such as process management, file descriptors, and how processes communicate with each other.

## Features
* Execute standard terminal commands
* Multi-stage pipeline support
    * ls | grep c | wc
* Dynamic command parsing
* Pipe creation and descriptor remapping with dup2()
* File descriptor cleanup
* Concurrent execution of pipeline processes
* Basic syntax error handling
* Dynamic memory allocation and cleanup

### Future Improvements
* I/O redirection (>, <)
* Built-in commands like cd
* Signal handling (Ctrl + C)
* More robust parsing and quote handling

### To Run
* Compile using "gcc main.c -o mini_shell"
* Run using "./mini_shell"
(mini_shell name is arbitrary)