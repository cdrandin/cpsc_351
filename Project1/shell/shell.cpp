// Christopher Randin 
// CPSC 351 Section #01

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
#include <vector>
#include <fstream>

#define SEPCHARS "\t \n"           //for strtok function
#define MAXCMD 100                 // total number of characters allow for command
#define MAXARG 80                  //total number of allowed arguments
#define SHELLNAME "CPSC351Shell:~" //name of shell (printed on each line)
    
void signalHandler( int signum );

// Check to see if arguments passed current location of the command has any pipes
bool is_there_next_pipe(char* arg[], const int& current, const int& total);

void process_pipe(int *counter);

// Determines if the command is cd, jobs, exit
bool is_internal_cmd(char* cmd);

void process_internal_cmd(char* cmd, char* arg[]);

void parse (char* cmd, char* arg[], int* c);
//This function takes the inupt and stores the arguments into
//the arg[] array for use in the calling of execvp

// Given the number of arguments and an array of arguments, this will execute
// those arguments.  The first argument in the array should be a command.
void run_cmd(char* arg[], int *c);

void pipe_cmd(char** cmd1, char** cmd2);

void name_shell(char path[]);

void print_jobs();


std::string shell_name;
std::vector<pid_t> proc_list;

int main(int argc, char *argv[])
{
    char cmd [MAXCMD];    
    char* arg [MAXARG];
    int* arg_count = new int(0),
       * current_count = new int(0);

    char the_path[256];
    getcwd(the_path, 255);
    name_shell(the_path);

    signal(SIGINT, signalHandler);

    system("clear");

    do
    {
        strcpy(cmd , "");   //clears the input (solves a problem I was having).

        printf("%s", shell_name.c_str());          //reprint shell name
        std::cin.getline(cmd, MAXCMD, '\n'); // get input

        parse(cmd, arg, arg_count);

        // Only processes stuff that was entered into the shell 
        if(strcmp(cmd, "\0") != 0)
        {
            if(is_there_next_pipe(arg, *current_count, *arg_count))
            {
                // TODO: Fix handling more than one pipe
                process_pipe(current_count);
                


                // seperate commands for piping
                char *cmd1[MAXARG], *cmd2[MAXARG];

                // Left program
                for(int i=0;i<*current_count;++i)
                {
                    cmd1[i] = arg[i];//cmd1[i] = arg[i];
                }

                // Right program
                for(int i=*current_count;i<*arg_count;++i)
                {
                    cmd2[i-(*current_count)] = arg[i];//cmd2[i-(*current_count)] = arg[i-(*current_count)];
                }

                pipe_cmd(cmd1, cmd2);

                //printf("Pipe: %s into %s\n", arg[*current_count-2], arg[*current_count]);
            }

            // Check if these are internal commands, which do not need forking
            else if(is_internal_cmd(cmd))
                process_internal_cmd(cmd, arg);
            
            // Run process
            else
                run_cmd(arg, arg_count);
        }
  
    } while(true); // "exit" will stop the shell

    return 0;
}



bool is_there_next_pipe(char* arg[], const int& current, const int& total)
{
    //std::cout << current << std::endl;

    for(int i=current;i<total;++i)
    {
        if(strcmp(arg[i], "|") == 0)
            return true;
    }
    return false;
}

bool is_internal_cmd(char* cmd)
{
    return (strcmp(cmd, "cd")   == 0 ||
            strcmp(cmd, "jobs") == 0 ||
            strcmp(cmd, "exit") == 0);
}

void process_internal_cmd(char* cmd, char* arg[])
{
    if(strcmp(cmd, "exit") == 0)
    {
        exit(1);
    }

    else if(strcmp(cmd, "cd") == 0)
    {
        char the_path[256];
        if (arg[1] == NULL)
            chdir ("/");

        else
            chdir (arg[1]);

        getcwd(the_path, 255);
        name_shell(the_path);

        perror (cmd);
    }

    else if(strcmp(cmd, "jobs") == 0)
    {
        print_jobs();
    }
}

void parse (char* cmd, char* arg[], int* c)
{
    int i = 0;
    arg[i] = strtok(cmd, SEPCHARS);
    i++;

    while(i<MAXARG && (arg[i] = strtok(NULL,SEPCHARS)) != NULL)
        i++;

    *c = i;
}

void run_cmd(char* arg[], int* c)
{
    pid_t pid;
    const char *amp = "&";
    bool found_amp = false;

    // found '&', so prepare to let it run in the background
    if(strcmp(arg[*c - 1], amp) == 0)
        found_amp = true;

    // forok process
    pid = fork();

    // error
    if (pid < 0)
        perror("Error (pid < 0)");

    // child process
    else if (pid == 0) 
    {
        // If the last argument is an ampersand, that's a special flag that we
        // don't want to pass on as one of the arguments.  Catch it and remove
        // it here.
        if (found_amp) 
        {
            arg[*c-1] = NULL;
        }

        execvp(arg[0], arg);
        perror("execvp error");
    } 

    else if (!found_amp)
        waitpid(pid, NULL, 0); // only wait if no ampersand

    else if(found_amp)
    {
        proc_list.push_back(pid);
        printf("[%d] %d\n", (int)proc_list.size(), pid);
    }
}

void process_pipe(int *counter)
{
    *counter += 2;


}

void pipe_cmd(char** cmd1, char** cmd2)
{
    int fds[2];
    pipe(fds);
    pid_t pid1, pid2;

    // child process #1
    if ((pid1 = fork()) == 0) 
    {
        dup2(fds[1], 1);
        close(fds[0]);
        close(fds[1]);
        execvp(cmd1[0], cmd1);

        perror("execvp failed");

        // child process #2
        } else if ((pid2 = fork()) == 0) {
          dup2(fds[0], 0);

          close(fds[1]);
          execvp(cmd2[0], cmd2);
          perror("execvp failed");
    }
    else 
    {
        close(fds[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }
}

// kill all processes in the background if shell is interupted
void signalHandler( int signum )
{
    for (std::vector<pid_t>::iterator it = proc_list.begin() ; it != proc_list.end(); ++it)
        kill(*it, SIGTERM);
    puts("");
    exit(signum);  
}

void name_shell(char path[])
{
    // Currently, update shell name based on current directory
    shell_name = SHELLNAME;
    shell_name += path;
    shell_name += "$ ";
}

void print_jobs()
{
    int i=1;

    for (std::vector<pid_t>::iterator it = proc_list.begin() ; it != proc_list.end(); ++it)
    {
        char *name = new char[30];
        if(name)
        {
            sprintf(name, "/proc/%d/cmdline", *it);
            std::ifstream infile(name);

            infile.getline(name, 30, '\n');

            if(std::string(name).length() != 0)
                printf("[%d]+\t   %s\t\t\t%s &\n", i, "Running", name);

            infile.close();
        }
        ++i;
    }
}
