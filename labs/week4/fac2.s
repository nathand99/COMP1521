# COMP1521 18s2 Week 04 Lab
# Compute factorials, iterative function


### Global data

   .data
msg1:
   .asciiz "n  = "
msg2:
   .asciiz "n! = "
eol:
   .asciiz "\n"

### main() function
   .text
   .globl main
main:
   #  set up stack frame
   sw    $fp, -4($sp)       # push $fp onto stack
   la    $fp, -4($sp)       # set up $fp for this function
   sw    $ra, -4($fp)       # save return address
   sw    $s0, -8($fp)       # save $s0 to use as ... int n;
   addi  $sp, $sp, -12      # reset $sp to last pushed item

   #  code for main()
   li    $s0, 0             # n = 0;
   
   la    $a0, msg1
   li    $v0, 4
   syscall                  # printf("n  = ");

   li    $v0, 5
   syscall                  # scanf("%d", into $v0)
   
   move $a0, $v0            #arg0 = n
# ... TODO: add your code here for  tmp = fac(n); ...
# ... place the parameter in $a0 and get the result from $v0 ...
   jal fac
   move $t0, $v0
   la    $a0, msg2
   li    $v0, 4
   syscall                  # printf("n! = ");

   move  $a0, $t0           # assume $t0 holds n!
   li    $v0, 1
   syscall                  # printf("%d\n",tmp);

   la    $a0, eol
   li    $v0, 4
   syscall                  # printf("\n");

   # clean up stack frame
   lw    $s0, -8($fp)       # restore $s0 value
   lw    $ra, -4($fp)       # restore $ra for return
   la    $sp, 4($fp)        # restore $sp (remove stack frame)
   lw    $fp, ($fp)          # restore $fp (remove stack frame)

   li    $v0, 0
   jr    $ra                # return 0

# fac() function

fac:
   # setup stack frame
   sw    $fp, -4($sp)       # push $fp onto stack
   la    $fp, -4($sp)       # set up $fp for this function
   sw    $ra, -4($fp)       # save return address
   sw    $s0, -8($fp)       # save $s0 to use as ... int i;
   sw    $s1, -12($fp)      # save $s1 to use as ... int prod;
   addi  $sp, $sp, -16      # reset $sp to last pushed item

   # code for fac()

# ... TODO: place your code for the body of fac() here ...
# ... use the value of n in $a0, place n! in $v0 ...

   li $t6,  1         #fac = 1
   li $t7,  1         #i = 1
                      #n = $a0
   LoopStart:
            bgt $t7, $a0, LoopEnd                #      if (i>n) goto LoopEnd;
            mul $t6, $t6, $t7                    #   fac = fac * i;
                                               
            addu $t7, $t7, 1                     #   i = i + 1;                
            j LoopStart                          #   goto LoopStart;
   LoopEnd:
   
   move $v0, $t6
   
   
   # clean up stack frame
   lw    $s1, -12($fp)      # restore $s1 value
   lw    $s0, -8($fp)       # restore $s0 value
   lw    $ra, -4($fp)       # restore $ra for return
   la    $sp, 4($fp)        # restore $sp (remove stack frame)
   lw    $fp, ($fp)         # restore $fp (remove stack frame)

   jr    $ra                # return prod;
