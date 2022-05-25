# COMP1521 18s2 Week 04 Lab
# Compute factorials, recursive function


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
   syscall
   move  $s0, $v0           # scanf("%d", &n);
   
   move  $a0, $s0           
   jal   fac
   nop
   move  $s0, $v0           # tmp = fac(n);
                            # recycle $s0, don't need n now
   la    $a0, msg2
   li    $v0, 4
   syscall                  # printf("n! = ");

   move  $a0, $s0
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
  # FUNCTION PROLOGUE
  # save values of stack frame
  sw $fp, -4($sp)  #store new frame pointer, 4 below current stack pointer.
  sw $ra, -8($sp)  #store return address, 8 below current stack pointer
  sw $s0, -12($sp) #store any s values that we plan to change on the stack.
  #change stack frame
  la $fp, -4($sp)   #move frame pointer, 4 below current stack pointer.
  add $sp, $sp, -12 #move stack pointer, 12 below current stack points (frame has $fp, $ra, $s0)


   # code for recursive fac()
   # n = $a0
   # we're going to store the future return value in $t5

   li $t0, 1                            # 1
   bgt $a0, $t0, return2                         # if (n > 1) {
                                                 # goto return2;
   li $v0, 1                                     # return 1;
   j end
   
return2:        
   move $s0, $a0                                 # s = n
   sub $a0, $a0, $t0                             # n = n - 1      
   
   jal fac
   mul $v0, $s0, $v0                             # return n * fac(n-1);
   j end

end:

  # FUNCTION EPILOGUE
  #move $v0, $v0   # place return value into $v0 (we've already done this)
  lw $s0, -8($fp) #restore the value of any saved s registers
  lw $ra, -4($fp) #restore the saved value of ra (return address)
  # remove stack frame
  la $sp, 4($fp)
  lw $fp, ($fp)

  #return
  jr    $ra                # return tmp;
