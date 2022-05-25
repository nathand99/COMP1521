    .data

newline:
    .asciiz "\n"
    .text
    .globl main
    
main:
    
    li $t0, 10
    li $t1, 20
    
    add $t3, $t0, $t1
    move $a0, $t3
    li $v0, 1
    syscall
    
    la $a0, newline
    li $v0, 4
    syscall 
    
    jal $ra
