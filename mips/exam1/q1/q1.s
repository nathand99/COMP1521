# COMP1521 Practice Prac Exam #1
# int rmOdd(int *src, int n, int*dest)

   .text
   .globl rmOdd

# params: src=$a0, n=$a1, dest=$a2
rmOdd:
# prologue
   addi $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   addi $sp, $sp, -4
   sw   $ra, ($sp)
   addi $sp, $sp, -4
   sw   $s0, ($sp)
   addi $sp, $sp, -4
   sw   $s1, ($sp)
   # if you need to save more $s? registers
   # add the code to save them here

# function body
# locals: 
#  $t0 = address of src
#  $t1 = address of dest
#  $t2 = address of n
#  $t3 = i
#  $t4 = j
#  $t5 = offset
#  $t6 = mod result

# put all parameters in variables
   move $t0, $a0
   move $t1, $a2
   move $t2, $a1
#   lw $t2, ($t2)

#   la $a0, ($t2)
#   li $v0, 1
#   syscall
   
   
   li $t3, 0            #i = 0
   li $t4, 0            #j = 0
   
loop:
   bgt $t3, $t2, end_loop   #if (i > n) {goto end_loop}
   
   mul $t5, $t3, 4          #t5 = i * 4
   add $t5, $t0, $t5        #src + t5 -> t0 now contains address on ith index in src
   lw $t5, ($t5)
   
   remu $t6, $t5, 2         #t6 = src[i]%2
   
   bgt $t6, 0, odd          # if number is odd, goto odd
   
   # NUMBER IS EVEN SO COPY TO DEST ARRAY 

    sw $t5, ($t1)
    addi $t1, $t1, 4
    
    addi $t4, $t4, 1
    
    
odd:
   addi $t3, $t3, 1
   b loop
end_loop:
   sub $t4, $t4, 1
   move $v0, $t4


# epilogue
   # if you saved more than two $s? registers
   # add the code to restore them here
   lw   $s1, ($sp)
   addi $sp, $sp, 4
   lw   $s0, ($sp)
   addi $sp, $sp, 4
   lw   $ra, ($sp)
   addi $sp, $sp, 4
   lw   $fp, ($sp)
   addi $sp, $sp, 4
   j    $ra

