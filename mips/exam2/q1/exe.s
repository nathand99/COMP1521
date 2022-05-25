# COMP1521 Practice Prac Exam #1
# arrays

   .data

a1:
   .word   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
a1N:
   .word   15     # int a1N = 15
a2:
   .space  60     # int a2[15]
a2N:
   .word   0      # int a2N
K:
   .word   5

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
   lw   $a2, K
   la   $a3, a2
   jal  everyKth     # a2N = everyKth(a1, a1N, K, a2)
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
# int everyKth(int *src, int n, int k, int*dest)

   .text
   .globl everyKth

# params: src=$a0, n=$a1, k=$a2, dest=$a3
everyKth:
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
# locals: ...

   # add code for your everyKth function here
   move $t0, $a0        #t0 = src
   move $t1, $a3        #t1 = dest
   move $t2, $a1        #t2 = n
   move $t3, $a2        #t3 = k
   
   li $t4, 0            #i = 0
   li $t7, 0            #j = 0

loop:
   bge $t4, $t2, end_loop       #if (i >= n) goto end_loop
   
   remu $t6, $t4, $t3
   bgt $t6, 0, continue
   
   #THIS IS KTH I
   
   lw $t5, ($t0)
   sw $t5, ($t1)
   
   addi $t1, $t1, 4
   addi $t7, $t7, 1
   
continue:

   addi $t4, $t4, 1
   addi $t0, $t0, 4
   j loop
end_loop:
   
   move $v0, $t7
   
   
   
   
   
   
   
   

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

