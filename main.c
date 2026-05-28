#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MSG "mini_shell > "

void free_argv_list(char*** argv_list, int argv_count);

int main(){

    while (true){
        write(1, MSG, sizeof(MSG) - 1);
        char buffer[1024];

        if (fgets(buffer, sizeof(buffer), stdin) != NULL){

            buffer[strcspn(buffer, "\r\n")] = 0;
            int pipes = 0;
            bool new_command = true;
            char*** argv_list = malloc(sizeof(char**)); //array that keeps track of pointers to the start of each arg array, argv**
            if (argv_list == NULL){
                perror("Malloc Error");
                exit(EXIT_FAILURE);
            }

            int argc = 1;
            char** argv = NULL;
            int argv_count = 1;

            char* token = strtok(buffer, " ");
            
            while (token != NULL){
                // strcmp(token, "|") == 0 TOKEN IS JUST A PIPE
                if (*token == '|'){  // can figure out how to evaluate things like ls |grep no space after check if points to | and next != null
                    if (argv_count == 1){
                        printf("Parse error. Try again.\n");

                        free_argv_list(argv_list, argv_count);
                        goto next_prompt;
                    }

                    argv[argc] = NULL;
                    
                    argv = NULL;
                    new_command = true;
                    pipes++;

                    if(token[1] != '\0'){
                        token++;
                        continue;
                    }
                }else {
                    if (new_command){
                        argc = 1;
                        argv = malloc(2 * sizeof(char*)); // allocate enough space for the null pointer at the end
                        argv[0] = token;
                        argv[1] = NULL;

                        argv_list[argv_count - 1] = argv;

                        argv_list = realloc(argv_list, (++argv_count) * sizeof(char**));
                        if (argv_list == NULL){
                            perror("Realloc Error");
                            exit(EXIT_FAILURE);
                        }

                    }else{
                        argv = realloc(argv, (argc + 2) * sizeof(char*));  
                        if (argv == NULL){
                            perror("Realloc Error");
                            exit(EXIT_FAILURE);
                        }

                        // updates stale pointer for argv
                        argv_list[argv_count - 2] = argv;

                        argv[argc++] = token;
                        argv[argc] = NULL;
                    }
                    new_command = false;
                }

                token = strtok(NULL, " ");
            }

            if (pipes == 0 && argv_list[0] != NULL && strcmp(*argv_list[0], "cd") == 0){
                const char* target = argv_list[0][1];
                if (target == NULL){
                    target = getenv("HOME");
                }else if (chdir(target) != 0){
                    perror("cd");
                }
                free_argv_list(argv_list, argv_count);

                goto next_prompt;
            }

            {
                int fd[pipes * 2];

                for(int i = 0; i < pipes; i++){
                    if(pipe(&fd[i * 2]) != 0){
                        perror("Failed Pipe");
                        exit(EXIT_FAILURE);
                    }
                }


                for(int command = 0; command < pipes + 1; command++){

                    pid_t pid = fork();

                    if (pid < 0){
                        printf("Failed fork.\n");
                        exit(EXIT_FAILURE);
                    } else if (pid == 0){
                        
                        if (command == 0 && pipes > 0){
                            // if command is first, but there are pipes
                            dup2(fd[1], STDOUT_FILENO); 

                        }else if (command == pipes && pipes > 0){
                            // last command
                            dup2(fd[(command - 1) * 2], STDIN_FILENO);


                        }else if (command > 0 && command < pipes) {
                            // middle command
                            dup2(fd[(command - 1) * 2], STDIN_FILENO);
                            dup2(fd[(command * 2) + 1], STDOUT_FILENO);

                        }

                        for(int i = 0; i < pipes * 2; i++){
                            close(fd[i]);
                        }

                        execvp(*argv_list[command], argv_list[command]);
                        perror("Failed Command");
                        
                        exit(EXIT_FAILURE);
                        // char fail_message[50];
                        // sprintf(fail_message, "command %d has failed\n", command);
                    }
                }   
                int rc; 
                for(int i = 0; i < pipes * 2; i++){
                    close(fd[i]);
                }
                while (waitpid(-1, &rc, 0) > 0);

            
                free_argv_list(argv_list, argv_count);
            }
            

            next_prompt:;
            
        } else{
            printf("\n"); 
            break;
        }
    }
    
}

void free_argv_list(char*** argv_list, int argv_count){
    for(int i = 0; i < argv_count; i++){
        free(argv_list[i]); // free(argv);
    }
    free(argv_list);
}