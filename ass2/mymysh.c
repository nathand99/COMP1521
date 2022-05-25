// mysh.c ... a small shell
// Started by John Shepherd, September 2018
// Completed by Nathan Driscoll z5204935, September/October 2018

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <glob.h>
#include <assert.h>
#include <fcntl.h>
#include "history.h"

// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(char *);

// Function forward references

void trim(char *);
int strContains(char *, char *);
char **tokenise(char *, char *);
char **fileNameExpand(char **);
void freeTokens(char **);
char *findExecutable(char *, char **);
int isExecutable(char *);
void prompt(void);

void execute(char **args, char **path, char **envp);

// Global Constants

#define MAXLINE 200
#define TRUE 1
#define FALSE 0
// Global Data

/* none ... unless you want some */


// Main program
// Set up enviroment and then run main loop
// - read command, execute command, repeat

int main(int argc, char *argv[], char *envp[])
{
   pid_t pid;   // pid of child process
   int stat;    // return status of child
   char **path; // array of directory names
   int cmdNo;   // command number
   int i;       // generic index

   // set up command PATH from environment variable
   for (i = 0; envp[i] != NULL; i++) {
      if (strncmp(envp[i], "PATH=", 5) == 0) break;
   }
   if (envp[i] == NULL)
      path = tokenise("/bin:/usr/bin",":");
   else
      // &envp[i][5] skips over "PATH=" prefix
      path = tokenise(&envp[i][5],":");
#ifdef DBUG
   for (i = 0; path[i] != NULL;i++)
      printf("path[%d] = %s\n",i,path[i]);
#endif

   // initialise command history
   // - use content of ~/.mymysh_history file if it exists

   cmdNo = initCommandHistory();
   // main loop: print prompt, read line, execute command

   char line[MAXLINE];
   prompt();
   while (fgets(line, MAXLINE, stdin) != NULL) {
      trim(line); // remove leading/trailing space
      
      //handle ! history substitution 
      // if user inputs !!, execute provious command
      if (line[0] == '!' && line[1] == '!') {
          if (cmdNo == 0) {
              printf("No previous command!\n");
              prompt();
              continue;
          }
          char *new_line = getCommandFromHistory(cmdNo);
          strcpy(line, new_line);
      }
      // find command with inputed seqNumber
      else if (line[0] == '!') {
          int command;
          sscanf(line, "!%d", &command);
          char *new_line = getCommandFromHistory(command);
                 
          if (new_line == NULL) {
              printf("No command #%d\n", command);
              prompt();
              continue;
          }
          else {
              strcpy(line, new_line);
          }
      }
      
      char **args; // array of command line arguments
      
      // split what is entered in the command line by spaces and put it in args
      args = tokenise(line, " ");
      
      //handle *?[~ filename expansion      
      args = fileNameExpand(args);

      // find number of tokens
      i = 0;
      while (args[i] != NULL) {
           i++;
       }
       
      // handle shell built-ins
      
      // exit: saveCommandHistory, then exit
      if (strcmp(args[0], "exit") == 0) {
          saveCommandHistory();
          printf("\n");
          exit(0);
      }
      
      //display the last 20 commands, with their sequence numbers
      else if (strcmp(args[0], "h") == 0 || strcmp(args[0], "history") == 0) {          
          showCommandHistory(stdout);
      }
      
      // print current working directory
      else if (strcmp(args[0], "pwd") == 0) {
          char cwd[1000];
          getcwd(cwd, 1000);
          printf("%s\n", cwd);
      }
      
      // change the shell's working directory then show new cwd
      else if (strcmp(args[0], "cd") == 0) {
          chdir(args[1]);
          char cwd[1000];
          getcwd(cwd, 1000);
          printf("%s\n", cwd);
      }

      // error if < and > in same line
      else if ((strstr(line, "<") != NULL) && (strstr(line, ">") != NULL)) {
                  printf("error");
              }
      else {
          int ret_value = 0;
          pid = fork();
          // child process - executes "execute" which does the command          
          if (pid == 0) {
              
              execute(args, path, envp);             
          }
          // parent process - waits for child process to finish
          else {
             wait(&stat); //parent needs to wait for child process to complete
             ret_value = WEXITSTATUS(stat); 
             printf("--------------------\nReturns %d\n", ret_value);
          }
          freeTokens(args);
      }

      addToCommandHistory(line, cmdNo);
      cmdNo++;
      prompt();
   }
   saveCommandHistory();
   //cleanCommandHistory();
   printf("\n");
   return(EXIT_SUCCESS);
}

// fileNameExpand: expand any wildcards in command-line args
// - returns a possibly larger set of tokens
char **fileNameExpand(char **tokens)
{
   // buffer for glob
   glob_t globbuf;
   
   // find number of tokens
   int count = 0;
   while (tokens[count] != NULL) { 
       count++;
   }
   
   // find number of tokens after expansion
   int count2 = 0;
   int k = 0;
   while (k < count) {
       if (strContains(tokens[k], "*?[~")) {
           glob(tokens[k], GLOB_NOCHECK|GLOB_TILDE, 0, &globbuf);
           count2 = count2 + globbuf.gl_pathc;
       }
       else {
           count2++;           
       }
       k++;
   }
   
   // expand any wildcards and put them in array expanded
   char **expanded = {NULL};
   expanded = malloc(count2 * 2 * sizeof(char*));
   int i = 0;
   int expanded_counter = 0;
   
   while (i < count) {
       // if contain a wildcard, glob it
       if (strContains(tokens[i], "*?[~")) {
       glob(tokens[i], GLOB_NOCHECK|GLOB_TILDE, 0, &globbuf);
       int j = 0;
       while (j < globbuf.gl_pathc) {
           char **globarray = globbuf.gl_pathv;
           expanded[expanded_counter] = globarray[j];
           expanded_counter++;
           j++;
       }
       }
       // if not, just add it to the expanded array
       else {
           expanded[expanded_counter] = tokens[i];
           expanded_counter++;
       }
       i++;
   }
   expanded[expanded_counter] = NULL;
   return expanded;
}

// findExecutable: look for executable in PATH
char *findExecutable(char *cmd, char **path)
{
      char executable[MAXLINE];
      executable[0] = '\0';
      if (cmd[0] == '/' || cmd[0] == '.') {
         strcpy(executable, cmd);
         if (!isExecutable(executable))
            executable[0] = '\0';
      }
      else {
         int i;
         for (i = 0; path[i] != NULL; i++) {
            sprintf(executable, "%s/%s", path[i], cmd);
            if (isExecutable(executable)) break;
         }
         if (path[i] == NULL) executable[0] = '\0';
      }
      if (executable[0] == '\0')
         return NULL;
      else
         return strdup(executable);
}

// isExecutable: check whether this process can execute a file
int isExecutable(char *cmd)
{
   struct stat s;
   // must be accessible
   if (stat(cmd, &s) < 0)
      return 0;
   // must be a regular file
   //if (!(s.st_mode & S_IFREG))
   if (!S_ISREG(s.st_mode))
      return 0;
   // if it's owner executable by us, ok
   if (s.st_uid == getuid() && s.st_mode & S_IXUSR)
      return 1;
   // if it's group executable by us, ok
   if (s.st_gid == getgid() && s.st_mode & S_IXGRP)
      return 1;
   // if it's other executable by us, ok
   if (s.st_mode & S_IXOTH)
      return 1;
   return 0;
}

// tokenise: split a string around a set of separators
// create an array of separate strings
// final array element contains NULL
char **tokenise(char *str, char *sep)
{
   // temp copy of string, because strtok() mangles it
   char *tmp;
   // count tokens
   tmp = strdup(str);
   int n = 0;
   strtok(tmp, sep); n++;
   while (strtok(NULL, sep) != NULL) n++;
   free(tmp);
   // allocate array for argv strings
   char **strings = malloc((n+1)*sizeof(char *));
   assert(strings != NULL);
   // now tokenise and fill array
   tmp = strdup(str);
   char *next; int i = 0;
   next = strtok(tmp, sep);
   strings[i++] = strdup(next);
   while ((next = strtok(NULL,sep)) != NULL)
      strings[i++] = strdup(next);
   strings[i] = NULL;
   free(tmp);
   return strings;
}

// freeTokens: free memory associated with array of tokens
void freeTokens(char **toks)
{
   for (int i = 0; toks[i] != NULL; i++)
      free(toks[i]);
   free(toks);
}

// trim: remove leading/trailing spaces from a string
void trim(char *str)
{
   int first, last;
   first = 0;
   while (isspace(str[first])) first++;
   last  = strlen(str)-1;
   while (isspace(str[last])) last--;
   int i, j = 0;
   for (i = first; i <= last; i++) str[j++] = str[i];
   str[j] = '\0';
}

// strContains: does the first string contain any char from 2nd string?
int strContains(char *str, char *chars)
{
   for (char *s = str; *s != '\0'; s++) {
      for (char *c = chars; *c != '\0'; c++) {
         if (*s == *c) return 1;
      }
   }
   return 0;
}

// prompt: print a shell prompt
// done as a function to allow switching to $PS1
void prompt(void)
{
   printf("mymysh$ ");
}

// execute: run a program, given command-line args, path (array of directory names) and envp
void execute(char **args, char **path, char **envp)
{
   // Program path to execute, whether executable found at path
   int i = 0;
   char *progpath;
   int found = FALSE;

   //if (args[0] starts with '/' or '.') {
   if ((args[0][0] == '.') || (args[0][0] == '/')){
       //   check if the file args[0] is executable
       //   if so, use args[0] as the command
       if (isExecutable(args[0])){
           found = TRUE;
           progpath = args[0];
       } else {
           found = FALSE;
       }
   }
   else {
     // For each of the directories D in the path
     // see if an executable file called "D/args[0]" exists
     // if it does, use that file name as the command

     for (i=0; path[i] != NULL; i++){
         char dirpath[100];
         strcpy(dirpath, path[i]);
         strcat(dirpath, "/");
         strcat(dirpath, args[0]);

        if (isExecutable(dirpath)){
            found = TRUE;
            progpath = dirpath;
            break;
        }
     }
   }
   
   // if (no executable file found)
   // print Command not found message
   if (!found) {
      printf("--------------------\n%s: Command not found\n", args[0]);
      
      //exit the child process (since execute() is not supposed to return)
      _Exit(EXIT_FAILURE);
   }
   // print the full name of the command being executed
   // use execve() to attempt to run the command with args and envp
   else {
       fprintf(stdout, "Running %s ...\n--------------------\n", progpath);
       
       // find out how many args
       i = 0;
       while (args[i] != NULL) {
           i++;
       }
       
       // sort out any input/output redirection
       if (i >= 2 && strstr(args[i - 2], "<") != NULL) {
           int fd0 = open(args[i - 1], O_RDONLY);
           dup2(fd0, STDIN_FILENO);
           args[i - 2] = NULL;
           args[i - 1] = NULL;
           close(fd0);
       }
       else if (i >= 2 && strstr(args[i - 2], ">") != NULL) {                 
           int fd1 = creat(args[i - 1] , 0644);
           dup2(fd1, STDOUT_FILENO);
           args[i - 2] = NULL;
           args[i - 1] = NULL;
           close(fd1);
       }
       
       // execute command here       
       int success = execve(progpath, args, envp);
       if (success == -1){
           perror("Exec failed");
           _Exit(EXIT_FAILURE);
       }

      // exit the child process (since execute() is not supposed to return)
     _Exit(EXIT_SUCCESS);
   }
}
