  
 .text
 .globl main

main: 
  
lw $t1, 0x8765

move $a0, $t1 
li   $v0, 1
syscall  

jr $ra
