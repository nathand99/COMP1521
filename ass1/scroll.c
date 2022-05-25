// COMP1521 18s2 Assignment 1
// Scroll letters from message in argv[1]

#include <stdlib.h>
#include <stdio.h>

// # of rows and columns in each big char
#define CHRSIZE 9
// number of rows in all matrices
#define NROWS 9
// number of columns in display matrix
#define NDCOLS 80
// max length of input string
#define MAXCHARS 100
// number of columns in bigString matrix
// max length of buffer to hold big version
// the +1 allows for one blank column between letters
#define NSCOLS (NROWS * MAXCHARS * (CHRSIZE+1))

#define CLEAR "\033[H\033[2J"

char theString[MAXCHARS+1];     // the input string (from command line)
char display[NROWS][NDCOLS];    // where text place before output
char bigString[NROWS][NSCOLS];  // big char version of input string

#include "chars.h"

void setUpDisplay(int starting, int slength);
void showDisplay();
void delay(int n);
int  isUpper(char ch);
int  isLower(char ch);

int main(int argc, char **argv)
{
   int  i, j;         // indexes
   int  row, col;     // indexes
   int  theLength;    // length of input string
   int  bigLength;    // length of the bigString
   int  iterations;
   int  starting_col;
   
   if (argc < 2) {
      printf("Usage: ./scroll String\n");
      return 1;
   }

   // copy the input string
   theLength = 0;
   for (char *s = argv[1]; *s != '\0'; s++) {
      char ch = *s;
      if (!isUpper(ch) && !isLower(ch) && ch != ' ') {
         printf("Only letters and spaces are allowed in the string!\n");
         exit(1);
      }
      if (theLength >= MAXCHARS) break;
      theString[theLength] = ch;
      theLength++;
   }
   theString[theLength] = '\0';
   bigLength = theLength * (CHRSIZE+1);

   // check the command-line arg (the string)
   if (theLength >= MAXCHARS) {
      printf("String must be < %d chars\n", MAXCHARS);
      return 1;
   }
   if (theLength < 1) {
      printf("Please enter a string with at least one character!\n");
      exit(1);
   }
    //START HERE
   // initialise the display to all spaces
   for (i = 0; i < NROWS; i++) {
      for (j = 0; j < NDCOLS; j++)
         display[i][j] = ' ';
   }

   // create the bigchars array
   for (i = 0; i < theLength; i++) {
      char ch = theString[i];
      if (ch == ' ') {
         for (row = 0; row < CHRSIZE; row++) {
             for (col = 0; col < CHRSIZE; col++)
                bigString[row][col + i * (CHRSIZE+1)] = ' ';
         }
      }
      else {
         int which;
         // calculate index of the char in all_chars
         if (isUpper(ch)) which = ch - 'A'; 
         if (isLower(ch)) which = ch - 'a' + 26;
         for (row = 0; row < CHRSIZE; row++) {
            for (col = 0; col < CHRSIZE; col++) {
               // copy char to the buffer
               bigString[row][col + i * (CHRSIZE+1)] = all_chars[which][row][col]; 
            }
         }
      }
      col = (i * (CHRSIZE+1)) + CHRSIZE;
      for (row = 0; row < CHRSIZE; row++)
         bigString[row][col] = ' ';
   }

   // enough to scroll it completely off the left
   iterations = NDCOLS+bigLength;
   
   // starting_col says how far across the display to start the text
   starting_col = NDCOLS-1;
   for (i = 0; i < iterations; i++) {
      setUpDisplay(starting_col, bigLength);
      showDisplay();
      // note that starting_col can go negative
      // in which case we show the string starting from part-way through
      starting_col--;
      delay(1);
   }
   
   return 0;
}

void setUpDisplay(int starting, int length)
{
   int row;     // current row in display and bigString
   int out_col; // index of current column in display
   int in_col;  // index of current column in bigString
   int first_col; // which column to start from in bigString

   if (starting < 0) {
      // start part-way through bigString
      // it's scrolling off the left of the display
      out_col = 0;
      first_col = -starting;
   }
   else {
      // blank out the part of the display before the string
      for (out_col = 0; out_col < starting; out_col++) {
         for (row = 0; row < NROWS; row++)
            display[row][out_col] = ' ';
      }
      first_col = 0;
   }
   // copy the relevant bits of the bigString into the display
   for (in_col = first_col; in_col < length; in_col++) {
      if (out_col >= NDCOLS) break;
      for (row = 0; row < NROWS; row++) {
         display[row][out_col] = bigString[row][in_col];
      }
      out_col++;
   }
}

// dump the contents of display[][] to stdout
void showDisplay()
{
   printf(CLEAR);
   for (int i = 0; i < NROWS; i++) {
      for (int j = 0; j < NDCOLS; j++)
         putchar(display[i][j]);
      putchar('\n');
   }
}

// waste some time
void delay(int n)
{
	int x = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 20000; j++)
            for (int k = 0; k < 1000; k++)
                x = x + 1;
    }
}

// check for an upper-case alphabetic
int isUpper(char ch)
{
   if (ch >= 'A' && ch <= 'Z')
      return 1;
   else
      return 0;
}

// check for a lower-case alphabetic
int isLower(char ch)
{
   if (ch >= 'a' && ch <= 'z')
      return 1;
   else
      return 0;
}
