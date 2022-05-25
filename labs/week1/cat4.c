// Copy input to output
// COMP1521 18s1

#include <stdlib.h>
#include <stdio.h>

void copy(FILE *, FILE *);

int main(int argc, char *argv[])
{
    if (argc == 1) {
	    copy(stdin,stdout);
	} else {
	    int i = 0;
	    for (i = 1; i < argc; i++) {
	        FILE *fp;
	        fp = fopen(argv[i], "r");
	        if (fp == NULL) {
	            printf("Can't read %s\n", argv[i]);
	            exit(1);
	        }
	        copy(fp, stdout);
	        fclose(fp);
	    }
	}
	return EXIT_SUCCESS;
}

// Copy contents of input to output, char-by-char
// Assumes both files open in appropriate mode

void copy(FILE *input, FILE *output)
{
    char c[BUFSIZ];
    while (fgets(c, BUFSIZ, input) != NULL) {
        fputs(c, output);
    }
}
