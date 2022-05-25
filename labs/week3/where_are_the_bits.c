// where_are_the_bits.c ... determine bit-field order
// COMP1521 Lab 03 Exercise
// Written by ...

#include <stdio.h>
#include <stdlib.h>

typedef struct _bit_fields {
   unsigned int a : 4,
                b : 8,
                c : 20;
} bfield;

/*
 
 unsigned int *p = (unsigned int *)bf;

 bfield: ordered c-b-a
 00000000000000000000 00000000 0000
 -------c------------ ----b--- --a-
 
 when a = 1, *p = 1
 00000000000000000000 00000000 0001
 
 when b = 1, *p = 16
 00000000000000000000 00000001 0000
 
 when c = 1, *p = 4096
 00000000000000000001 00000000 0000
 
 when b = 1, a = 1, *p = 17
 00000000000000000000 00000001 0001
 
*/

int main(void)
{
  bfield *bf = (bfield *)malloc(sizeof(bfield));
  
  bf->a = 1;
  bf->b = 1;
  bf->c = 0;
  
  unsigned int *p = (unsigned int *)bf;
  
  printf("p = %d\n", *p);
  
  printf("%u\n",sizeof(bf));

  return 0;
}

