// make_a_float ... read in bit strings to build a float value
// COMP1521 Lab03 exercises
// Written by John Shepherd, August 2017
// Completed by Dan Kennedy & Nathan Driscoll

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef uint32_t Word;

//Float32
struct _float {
   // define bit_fields for sign, exp and frac
   // obviously they need to be larger than 1-bit each
   // and may need to be defined in a different order
  // DAN'S NOTE: defining them backwards here (frac first)
  // so they're in the right order in memory.
  unsigned int frac:23, exp:8, sign:1;
};
typedef struct _float Float32;

//Union of Float32, Word (typedefed uint32_t), and float (normal float)
//Called "Union32".
union _bits32 {
   float   fval;  // interpret the bits as a float
   Word    xval;  // interpret as a single 32-bit word
   Float32 bits;  // manipulate individual bits
};
typedef union _bits32 Union32;

//Function signatures
void    checkArgs(int, char **);
Union32 getBits(char *, char *, char *);
char   *showBits(Word, char *);
int     justBits(char *, int);


int main(int argc, char **argv)
{
   union _bits32 u;
   char out[50];

   u.bits.sign = u.bits.exp = u.bits.frac = 0;

   // check command-line args (all strings of 0/1
   // kills program if args are bad
   checkArgs(argc, argv);

   // convert command-line args into components of
   // a Float32 inside a Union32, and return the union
   u = getBits(argv[1], argv[2], argv[3]);

   printf("bits : %s\n", showBits(u.xval,out));
   printf("float: %0.10f\n", u.fval);

   return 0;
}

//getBits: takes bit-strings from the command-line and stores
//them in the appropriate components in a Union32 object

// convert three bit-strings (already checked)
// into the components of a struct _float
Union32 getBits(char *sign, char *exp, char *frac)
{

  // to return
  Union32 new;
  
  // 1) If sign is '0', new.bits.sign = 0, otherwise 1.
  new.bits.sign = (*sign == '0')? 0 : 1;
  
  // 2) Convert char *exp into an 8-bit value in new.bits
  unsigned int mask = 1;
  unsigned int val = 0;  //we can use this to set exp, because sizeof(exp) = 8
  int i = 7;
  while (i>=0){
    //E.g. if *exp is 128 ('1','0','0','0','0','0','0','0'),
    //then on the first iteration we'll have
    //0000 0000 OR 1 << 7, (i.e. 1000 0000)
    //and so val will be set to 1000 0000, i.e. 128.
    if (*exp == '1'){
      val |= mask << i;
    }
    exp++;
    i--;
  }
  new.bits.exp = val;
  
  // 3) convert char *frac into a 23-bit value in new.bits
  
  // Need to make a special struct for masking here
  // because of random 23 bit length of frac
  Union32 mask_new;
  new.bits.frac = 0;
  mask_new.bits.frac = 1;
  
  i = 22;
  while (i>=0){
    
    if (*frac == '1'){
      new.bits.frac |= mask_new.bits.frac << i;
    }
    frac++;
    i--;
  }

   return new;
}

// convert a 32-bit bit-stringin val into a sequence
// of '0' and '1' characters in an array buf
// assume that buf has size > 32
// return a pointer to buf
char *showBits(Word val, char *buf)
{
  //32 chars, to represent bits with '1' or '0'
  char temp[32];
  
  //Read each bit of Word into array as chars '1' or '0'
  unsigned int mask = 1 << 31; // i.e. 10000.......0 (32 bit unsigned int)
  int i;
  for (i=0; i<33; i++) {
    //If there's a bit at position [i], put a '1' into temp[i], otherwise put '0'.
    temp[i] = ((val & (mask >> i)) > 0)? '1':'0';
  }
  
  //Put in spaces as required by printing
  int j = 0;
  for (i=0; i<34; i++) {
    if (i==1 || i==10){
      buf[i] = ' ';
    }
    else {
      buf[i] = temp[j];
      j++;
    }
  }
  buf[34] = '\0';
  
  return buf;
}

// checks command-line args
// need at least 3, and all must be strings of 0/1
// never returns if it finds a problem
void checkArgs(int argc, char **argv)
{
   if (argc < 3) {
      printf("Usage: %s Sign Exp Frac\n", argv[0]);
      exit(1);
   }
   //Sign portion is one bit (1 = negative, 0 = positive)
   if (!justBits(argv[1],1)) {
      printf("%s: invalid Sign: %s\n", argv[0], argv[1]);
      exit(1);
   }
   //Exponent portion is 8 bits long (unsigned char, essentially)
   if (!justBits(argv[2],8)) {
      printf("%s: invalid Exp: %s\n", argv[0], argv[2]);
      exit(1);
   }
   //Fraction portion is 23 bits long
   if (!justBits(argv[3],23)) {
      printf("%s: invalid Frac: %s\n", argv[0], argv[3]);
      exit(1);
   }
   return;
}

// check whether a string is all 0/1 and of a given length
int justBits(char *str, int len)
{
   if (strlen(str) != len) return 0;

   while (*str != '\0') {
      if (*str != '0' && *str != '1') return 0;
      str++;
   }
   return 1;
}


//Notes from Videos

//
//  //can initialise a bitfield like struct _bit_fields x = {0, 0, 0};
//
//  unsigned int *ptrX = (unsigned int *) &x;
//
//  unsigned int val 10;
//
//  unsigned int *valPtr = &val;
//
//  unsigned int mask =1;
//  int i = 0;
//
//  //here we're going to do an "and" operation.
//  //Returns 1 only if both bits are set to 1.
//  while (i<32) {
//
//    //if an "and" operation with the bitmask and the value at the pointer
//    //returns a number, we know that there's a bit there.
//    if ((*valPtr & mask) > 0 ){
//      //means there's a bit here
//    }
//
//
//    i++;
//    mask = mask << 1;
//
//
//  //Bitstring is a set of 0 1.
//  //union: allows you to have multiple types for one variable in C.
//  //do getBits, then showbits.

