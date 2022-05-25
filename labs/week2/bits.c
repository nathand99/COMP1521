// COMP1521 18s1 Week 02 Lab (warm-up)

#include <stdio.h>

int main()
{
	// Code to generate and display the largest "int" value

	int x = 1;
	x = ~(x << 31);
	printf("int %x, %d\n", x, x);
	
	//we want 011111..111 (0 at the front means positive and the rest ones means 
	//all other numbers are filled in. So we get 000..001 which is 1 and << the 1
	//all the way to the front (31 spots). Which gives us the maximum negative number
	//(111111111..111 is negative 1) max negative number is 100000000. Now we just ~ IT TO GET
	//what we want

	// Code to generate and display the largest "unsigned int" value
	
	// for unsigned data types, we don't have to worry about negative numbers
	// so we get the smallest value (0) and negate it to make it the max number
	// this does not change for int, long, long long etc.... and long as unsigned

	unsigned int y = ~0;

	printf("unsigned int %x, %u\n", y, y);

	// Code to generate and display the largest "long int" value
    // a long int behaves the same as a normal int
    
	long int xx = 1;

	xx = ~(xx << 31);
	printf("long int %lx %ld\n", xx, xx);

	// Code to generate and display the largest "unsigned long int" value

	unsigned long int xy = ~0;
	printf("unsigned long int %lx, %lu\n", xy, xy);

	// Code to generate and display the largest "long long int" value
    // we need to shift a long long int double the amount as int and + 1
	long long int xxx = 1;
	xxx = ~(xxx << 63);
	printf("long long int %llx, %llu\n", xxx, xxx);

	// Code to generate and display the largest "unsigned long long int" value

	unsigned long long int xxy = ~0;
	printf("unsigned long long int %llx, %llu\n", xxy, xxy);

	return 0;
}

