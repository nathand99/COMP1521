# COMP1521 Practice Prac Exam #1
# int lowerfy(char *src, char *dest)

   .text
   .globl lowerfy

# params: src=$a0, dest=$a1
lowerfy:
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

   move $t0, $a0        #t0 = src
   move $t1, $a1        #t1 = dest

   li $t2, 0            #$t2 = i = 0
   li $t3, 0            #n = 0
   
loop:
   lb $t4, ($t0)
   beqz $t4, end_loop   #if (src[i] == '\0') goto end_loop
   
   blt $t4, 'A', continue         #goto continue
   bgt $t4, 'Z', continue         #goto continue
   
   # LETTER IS UPPERCASE
   sub $t4, $t4, 'A'
   add $t4, $t4, 'a'
   addi $t3, $t3, 1

continue:
   sb $t4, ($t1)
   
   addi $t0, $t0, 1
   addi $t1, $t1, 1
   j loop
end_loop:
   sb $0, ($t1)
   move $v0, $t3
   
   
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

