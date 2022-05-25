# COMP1521 Practice Prac Exam #1
# arrays

   .data

a1:
   .word   1, 1, 1, 1, 1
a1N:
   .word   5      # int a1N = 5
a2:
   .space  20     # int a2[5]
a2N:
   .word   0      # int a2N = 0

   .align  2
# COMP1521 Practice Prac Exam #1
# main program + show function

   .data
m1:
   .asciiz "a1 = "
m2:
   .asciiz "a2 = "
   .align  2

   .text
   .globl main
main:
   addi $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   addi $sp, $sp, -4
   sw   $ra, ($sp)

   la   $a0, m1
   li   $v0, 4
   syscall           # printf("a1 = ")
   la   $a0, a1
   lw   $a1, a1N
   jal  showArray    # showArray(a1, a1N)

   la   $a0, a1
   lw   $a1, a1N
   la   $a2, a2
   jal  rmOdd        # a2N = rmOdd(a1, a1N, a2)
   sw   $v0, a2N


   la   $a0, m1
   li   $v0, 4
   syscall           # printf("a1 = ")
   la   $a0, a1
   lw   $a1, a1N
   jal  showArray    # showArray(a1, a1N)

   la   $a0, m2
   li   $v0, 4
   syscall           # printf("a2 = ")
   la   $a0, a2
   lw   $a1, a2N
   jal  showArray    # showArray(a2, a2N)

   lw   $ra, ($sp)
   addi $sp, $sp, 4
   lw   $fp, ($sp)
   addi $sp, $sp, 4
   j    $ra

# params: a=$a0, n=$a1
# locals: a=$s0, n=$s1, i=$s2
showArray:
   addi $sp, $sp, -4
   sw   $fp, ($sp)
   la   $fp, ($sp)
   addi $sp, $sp, -4
   sw   $ra, ($sp)
   addi $sp, $sp, -4
   sw   $s0, ($sp)
   addi $sp, $sp, -4
   sw   $s1, ($sp)
   addi $sp, $sp, -4
   sw   $s2, ($sp)

   move $s0, $a0
   move $s1, $a1
   li   $s2, 0            # i = 0
show_for:
   bge  $s2, $s1, end_show_for

   move $t0, $s2
   mul  $t0, $t0, 4
   add  $t0, $t0, $s0
   lw   $a0, ($t0)
   li   $v0, 1            # printf("%d",a[i])
   syscall

   move $t0, $s2
   addi $t0, $t0, 1
   bge  $t0, $s1, incr_show_for
   li   $a0, ','
   li   $v0, 11           # printf(",")
   syscall

incr_show_for:
   addi $s2, $s2, 1       # i++
   j    show_for

end_show_for:
   li   $a0, '\n'
   li   $v0, 11
   syscall

   lw   $s2, ($sp)
   addi $sp, $sp, 4
   lw   $s1, ($sp)
   addi $sp, $sp, 4
   lw   $s0, ($sp)
   addi $sp, $sp, 4
   lw   $ra, ($sp)
   addi $sp, $sp, 4
   lw   $fp, ($sp)
   addi $sp, $sp, 4
   j    $ra
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

