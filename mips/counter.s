    .data

nl:
    .asciiz "\n"
    .text
    .globl main
    
main:
    li $t0, 0   #t0 = i = 0
    
loop:
    bgt $t0, 10, end_loop
    
    #print the number
    move $a0, $t0
    li $v0, 1
    syscall
    
    #print newline
    la $a0, nl
    li $v0, 4
    syscall
    
        
    addi $t0, 1
    j loop
    
end_loop:
    
    jr $ra
