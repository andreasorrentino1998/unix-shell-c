// SHELL UNIX v.1.0
// Author: Andrea Sorrentino
// Date: 24/10/18

// LIBRARIES
#include <cstdlib>      // Provides malloc(), free()
#include <stdio.h>      // Provides fgets(), printf() and scanf()
#include <string.h>     // Provides strlen(), strcmp(), strtok(), strcpy(), strcat(), strrchr(), memcpy()
#include <unistd.h>     // Provides fork(), getcwd() and chdir()
#include <errno.h>      // Provides errno and strerror()
#include <sys/wait.h>   // Provides wait() and execv()

// CONSTANTS
#define COMMAND_MAX_LENGTH 1000
#define DIRECTORY_MAX_LENGTH 100
#define MAX_ARG_NUM 100

// FUNCTION PROTOTYPES
void parse_command(char *command, char **args, int *n_tokens);
void execute_command(char **args, const int *n_tokens, char *working_dir);

int main(){
    // Working directory, command and arguments strings, number of arguments
    char *working_dir = (char *) malloc(DIRECTORY_MAX_LENGTH * sizeof(char));
    char *command = (char *) malloc(sizeof(char));
    char *args[MAX_ARG_NUM];
    int n_tokens = 0;

    // Clear the console
    strcpy(command, "clear");
    parse_command(command, args, &n_tokens);
    execute_command(args, &n_tokens, NULL);

    // Print the welcome text
    printf("SHELL UNIX v.1.0\n");
    printf("Author: Andrea Sorrentino\n\n\n");

    // Get the current working directory from the system
    getcwd(working_dir, DIRECTORY_MAX_LENGTH);


    // Scan and execute command until the user doesn't insert "quit" to exit.
    do {
        printf("%s> ", working_dir);
        fgets(command, COMMAND_MAX_LENGTH, stdin);  // Get the user input (the last character is a '\n')
        command[strlen(command) - 1] = '\0';        // Replace '\n' with '\0'
        parse_command(command, args, &n_tokens);
        execute_command(args, &n_tokens, working_dir);
    } while(strcmp(command, "quit"));

    free(command);
    free(working_dir);
    return 0;
}

// PRECONDITION: None
// POSTCONDITION: Scan the command subdiving it in tokens.
// The pointer **args will point to the array of tokens extracted
// from the command pointed by *command. The pointer *n_tokens will
// point to the number of tokens extracted form the command.
// The pointer *command will be not modified.
void parse_command(char *command, char **args, int *n_tokens){
    *n_tokens = 0;
    args[*n_tokens] = strtok(command, " ");
    while(args[*n_tokens] != NULL){
        *n_tokens = *n_tokens + 1;
        args[*n_tokens] = strtok(NULL, " ");
    }
}

// PRECONDITION: The pointer **args must point to an array of strings,
// where each element contains a token of the command. The pointer
// *n_tokens must contain the tokens number of the command.
// POSTCONDITION: Execute the command. If the command is wrong
// or the execution fails for other reasons, it will display an error.
// The input parameters of the function will be not modified.
// If the command is "cd", it could change the value pointed by *working_dir.
void execute_command(char **args, const int *n_tokens, char *working_dir){
    // If the first token is empty, return
    if(args[0] == NULL) return;

    // If the command is "quit", return
    if(!strcmp(args[0], "quit")) return;

    // If the command is "cd", update the working directory
    if(!strcmp(args[0], "cd")){
        char *new_working_dir = (char*) malloc(DIRECTORY_MAX_LENGTH * sizeof(char));
        if(args[1] == NULL) {
            // Go back of one folder
            if(!strcmp(working_dir, "/")) return;
            char *last_folder = strrchr(working_dir, '/');
            memcpy(new_working_dir, working_dir, last_folder - working_dir);
            new_working_dir[last_folder-working_dir] = '\0';
        }
        else {
            // Link the two paths
            new_working_dir = strcpy(new_working_dir, working_dir);
            new_working_dir = strcat(new_working_dir, "/");
            new_working_dir = strcat(new_working_dir, args[1]);
        }
        // Set the new directory. If the directory doesn't exists
        // or is not accessible print an error
        if(chdir(new_working_dir) == -1) printf("Error n.(%d): %s\n", errno, strerror(errno));
        else{
            strcpy(working_dir, new_working_dir);
            free(new_working_dir);
        }
        return;
    }

    // Fork a new process
    pid_t pid;
    switch(pid = fork()){
        case -1:
            printf("Fork failed!");
            break;
        case 0:
            // Create the path "/bin/"
            char *path = (char *) malloc((5 + strlen(args[0])) * sizeof(char));
            strcpy(path, "/bin/");
            strcpy(path+5, args[0]);

            // Execute the task
            int result = execv(path, args);
            if(result == -1){   // If the command to execute is not in this directory
                // Change the path to "/usr/bin/"
                free(path);
                path = (char *) malloc((9 + strlen(args[0])) * sizeof(char));
                strcpy(path, "/usr/bin/");
                strcpy(path+9, args[0]);

                // Execute the task again
                result = execv(path, args);

                // If the execution fails, print the error
                if(result == -1) printf("Error n.(%d): %s\n", errno, strerror(errno));

                free(path);
                return;
            }
            break;
    }
    // Stop the shell until the task has not termined his execution
    wait(NULL); 
}
