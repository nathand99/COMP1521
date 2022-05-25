// COMP1521 18s2 mysh ... command history
// Implements an abstract data object

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

// This is defined in string.h
// BUT ONLY if you use -std=gnu99
//extern char *strdup(const char *s);

// Command History
// array of command lines
// each is associated with a sequence number

#define MAXHIST 20
#define MAXSTR  200

#define HISTFILE ".mymysh_history"

typedef struct _history_entry {
   int   seqNumber;
   char *commandLine;
} HistoryEntry;

typedef struct _history_list {
   int nEntries;
   HistoryEntry commands[MAXHIST];
} HistoryList;

HistoryList CommandHistory;

// initCommandHistory()
// - initialise the data structure
// - read from .history if it exists
// return number seqNumber of the last command

int initCommandHistory()
{
   HistoryList *new = malloc (sizeof(HistoryList));
   new = &CommandHistory;
   
   CommandHistory.nEntries = 0;
   
   char *name = "HOME";
   char *value;
   value = getenv(name);
   sprintf(value, "%s/%s", value, HISTFILE);
   
   FILE *fp;
   fp = fopen(value, "w");
   int last_seq = 0;
   char line[MAXSTR];
   char temp[MAXSTR];
   int i = 0;
   while (fgets(line, MAXSTR, fp) != NULL) {
       sscanf(line, " %3d  %s\n", &new->commands[i].seqNumber, temp);
       new->nEntries = i + 1;
       new->commands[i].commandLine = malloc(sizeof(temp) + 1);
       strcpy(new->commands[i].commandLine, temp);       
       last_seq = new->commands[i].seqNumber;
       i++;
   }
   fclose(fp);
   return last_seq;
}

// addToCommandHistory()
// - add a command line to the history list
// - overwrite oldest entry if buffer is full

void addToCommandHistory(char *cmdLine, int seqNo)
{
   int size = strlen(cmdLine) + 1;
   // if CommandHistory is full, overwrite oldest buffer (first one)
   if (CommandHistory.nEntries >= 20) {       
       // move every command up the array and then insert command at end
       int i = 0;
       while (i < 19) {           
           int new_size = sizeof(CommandHistory.commands[i + 1].commandLine) + 1;
           CommandHistory.commands[i].commandLine = realloc(CommandHistory.commands[i].commandLine, new_size);
           strcpy(CommandHistory.commands[i].commandLine, CommandHistory.commands[i + 1].commandLine);  
           CommandHistory.commands[i].seqNumber = CommandHistory.commands[i + 1].seqNumber;
           i++;    
       }      
       CommandHistory.commands[19].commandLine = malloc(sizeof(size));
       strcpy(CommandHistory.commands[19].commandLine, cmdLine);
       CommandHistory.commands[19].seqNumber = seqNo + 1;
   }
   // if CommandHistory is empty, put this command in
   else if (CommandHistory.nEntries == 0) {
       CommandHistory.nEntries = 1;
       CommandHistory.commands[0].commandLine = malloc(sizeof(size));
       strcpy(CommandHistory.commands[0].commandLine, cmdLine);
       CommandHistory.commands[0].seqNumber = 1;
   }
   // else, add it on the end of the array
   else {   
       int entry = CommandHistory.nEntries;
       CommandHistory.commands[entry].commandLine = malloc(sizeof(size));
       strcpy(CommandHistory.commands[entry].commandLine, cmdLine);
       CommandHistory.commands[entry].seqNumber = seqNo + 1;
       CommandHistory.nEntries++;
   }
}

// showCommandHistory()
// - display the list of 

void showCommandHistory(FILE *outf)
{
   int i = 0;
  // printf("%d",CommandHistory.nEntries); 
   while (i < CommandHistory.nEntries) {
        fprintf(outf, " %3d  %s\n", CommandHistory.commands[i].seqNumber, CommandHistory.commands[i].commandLine);
        i++;
    }
}

// getCommandFromHistory()
// - get the command line for specified command
// - returns NULL if no command with this number

char *getCommandFromHistory(int cmdNo)
{
    if (cmdNo > CommandHistory.nEntries) {
        return NULL;
    }
    
    int i = 0;
    while (i < CommandHistory.nEntries) {
        if (CommandHistory.commands[i].seqNumber == cmdNo) {
            return CommandHistory.commands[i].commandLine;
        }
        i++;
    }
    
    return NULL;
}

// saveCommandHistory()
// - write history to $HOME/.mymysh_history

void saveCommandHistory()
{
   char *name = "HOME";
   char *value;
   value = getenv(name);
   FILE *fp2;
   fp2 = fopen(value, "w");
   fprintf(fp2, "heloooooooo");
   int i = 0;
   while (i < CommandHistory.nEntries) {
       
       fprintf(fp2, " %3d  %s\n", CommandHistory.commands[i].seqNumber, CommandHistory.commands[i].commandLine);
       i++;
   }
   free(fp2);
}

// cleanCommandHistory
// - release all data allocated to command history

void cleanCommandHistory()
{
    int i = 0;
    while (i < CommandHistory.nEntries) {
        free(CommandHistory.commands[i].commandLine);
        i++;
    }
   // free(CommandHistory);
   // TODO
}
