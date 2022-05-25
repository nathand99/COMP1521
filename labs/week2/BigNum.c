// BigNum.h ... LARGE positive integer values

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "BigNum.h"

// Initialise a BigNum to N bytes, all zero
void initBigNum(BigNum *n, int Nbytes)
{
   // malloc space for a char array of nbytes size 
   // assert that malloc did not return NULL
   Byte *p = malloc(Nbytes * sizeof(Byte));
   assert(p != NULL);
   n -> bytes = p;
   n -> nbytes = Nbytes;
   
   // initialise char array to 0
   int i = 0;
   while (i < Nbytes) {
       n -> bytes[i] = 0;
       i = i + 1;
   }
   return;
}

// Add two BigNums and store result in a third BigNum
void addBigNums(BigNum n, BigNum m, BigNum *res)
{
   //compute likely size of sum array needed
  int max = (m.nbytes > n.nbytes)? m.nbytes : n.nbytes;

  //array to hold sum, pointer to start of array
  //Not possible to get a number more than one digit larger than max
  Byte *array = malloc((max + 1) * sizeof(Byte));
  Byte *p = array;

  //Perform addition in old fashioned way, with carry
  int num1, num2, sum, digit;
  int carry = 0;
  int i;

  for (i = 0; i < max+1; i++) {

    //Either turn char into digit, or give it value of zero
    num1 = (i < n.nbytes)? n.bytes[i] - '0' : 0;
    num2 = (i < m.nbytes)? m.bytes[i] - '0' : 0;

    sum = num1 + num2 + carry;
    digit = (sum<10)? sum : sum-10;
    carry = (sum<10)? 0 : 1;

    //turn digit back into a char, add to res array, increment pointer
    *p = digit + '0';
    p++;

  }

  // Get length of computed sum
  int sum_length = 1;
  int k;
  for (k = 0; k < max; k++) {
    sum_length++;
  }

  //add array to res struct
  res->bytes = array;
  res->nbytes = sum_length;
   return;
}

// Set the value of a BigNum from a string of digits
// Returns 1 if it *was* a string of digits, 0 otherwise
int scanBigNum(char *s, BigNum *n)
{
    // fix up the number given to us in char *s
    // remove leading 0's and spaces
    
    // copy s into c to fix up s
    char *c = 0;
    c = s;
   
   int i = 0;
   int length = 0;
   int number_started = 0;
   while (c[i] != '\0') {
       // if it is a digit the number has started
       if (!(number_started) && (isdigit(c[i]))) {
           number_started = 1;
       }
       // remove leading spaces and zeros by moving pointer to start
       // of number to the next digit
       if ((!(number_started)) && ((isspace(c[i])) || (c[i] == '0'))) {
           s++;
       } 
       // if there is a space at the end, put a '\0' to end the array
       else if (number_started && isspace(c[i])) {
           c[i] = '\0';
       } 
       // non-digits not allowed, return 0
       else if (!(isdigit(c[i]))) {
           return 0;
       } 
       // count the length of the digit
       else {
           length++;
       }
       i++;
   }
   
   // check to see if the array in BigNum is big enough to hold the number
   // malloc a new array thats big enough if it isn't
   if (length > n -> nbytes) {
       Byte *b = malloc(length * sizeof(Byte));
       assert(b != NULL);
       n -> nbytes = length;
       n -> bytes = b;
   }
   
   // put the number into the bytes array backwards
   i = length;
   int j = 0;
   while (j < n -> nbytes) {
       if (i > 0) {
           n -> bytes[j] = s[i - 1];
       }
       else {
           n -> bytes[j] = '0';
       }
       i--;
       j++;
   }
   return 1;
}

// Display a BigNum in decimal format

void showBigNum(BigNum n)
{
   int i = n.nbytes - 1;
   int startnum = 0;
   while (i >= 0) {
       if (!(n.bytes[i] == '0')){
           //printf("8%c8", n.bytes[i]);
           startnum = 1;
       }
       if ((startnum)){
       printf("%c", n.bytes[i]);
       //printf(" %d ", i);
       //printf(" 6%d6 ", startnum);
       }
       i = i - 1;
   }
   
   return;
}
/*

void showBigNum(BigNum n)
{
  //Pointer to end of bytes array
  Byte *b = n.bytes + (n.nbytes-1);

  int i = 0;
  int startnum = 0; //indicates the number has been reached, not a padding zero

  for (i = 0; i<n.nbytes; i++){
    if (!(*b == '0')){
      startnum = 1;
    }
    if (startnum){
      printf("%c", *b);
    }
    b--;
  }

  return;
}
*/
