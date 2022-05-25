// make_a_float ... read in bit strings to build a float value
// COMP1521 Lab03 exercises
// Written by John Shepherd, August 2017
// Completed by ...

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef uint32_t Word;

struct _float {
   // define bit_fields for sign, exp and frac
   // obviously they need to be larger than 1-bit each
   // and may need to be defined in a different order
   unsigned int frac:23, exp:8, sign:1;
};
typedef struct _float Float32;

union _bits32 {
   float   fval;  // interpret the bits as a float
   Word    xval;  // interpret as a single 32-bit word
   Float32 bits;  // manipulate individual bits
};
typedef union _bits32 Union32;

void    checkArgs(int, char **);
Union32 getBits(char *, char *, char *);
char   *showBits(Word, char *);
int     justBits(char *, int);

int main(int argc, char **argv)
{
   union _bits32 u;
   char out[50];

   // here's a hint ...
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

// convert three bit-strings (already checked)
// into the components of a struct _float
Union32 getBits(char *sign, char *exp, char *frac)
{
   Union32 new;

   // convert char *sign into a single bit in new.bits
   // if sign is 0, then make it 0, if its 1, make it 1
   if (sign == '0') {
       new.bits.sign = 0;
   } else {
       new.bits.sign = 1;
   }
   // convert char *exp into an 8-bit value in new.bits
   // we want to convert a char * string into bits
    unsigned int mask = 1;
    unsigned int val = 0;  //we can use this to set exp, because sizeof(exp) = 8
    int i = 7;
    while (i>=0){
      //E.g. if *exp is 128 ('1','0','0','0','0','0','0','0'),
      //then on the first iteration we'll have
      //0000 0000 OR 1 << 7, (i.e. 1000 0000) (0000 0000 | 1000 0000)
      //and so val will be set to 1000 0000, i.e. 128.
    if (*exp == '1'){
      val |= mask << i;
    }
    exp++;
    i--;
  }
  new.bits.exp = val;
   // convert char *frac into a 23-bit value in new.bits
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
   if (!justBits(argv[1],1)) {
      printf("%s: invalid Sign: %s\n", argv[0], argv[1]);
      exit(1);
   }
   if (!justBits(argv[2],8)) {
      printf("%s: invalid Exp: %s\n", argv[0], argv[2]);
      exit(1);
   }
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
