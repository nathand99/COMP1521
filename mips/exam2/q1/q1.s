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

