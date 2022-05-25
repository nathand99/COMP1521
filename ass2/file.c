
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

int main (void) {
char *name = "HOME";
   char *value;
   value = getenv(name);
   printf("---------%s------", value);
   FILE *fp;
   fp = fopen(value, "w");
 //  fprintf(fp, "heloooooooo");
   free(fp);
   }
