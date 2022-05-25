############################################################ -*- asm -*-
# COMP1521 18s2 -- Assignment 1 -- Scrolling Text!
# Scroll letters from a message in argv[1]
#
# Base code by Jashank Jeremy
# Tweaked by John Shepherd
# $Revision: 1.5 $
#
# Edit me with 8-column tabs!

# Requires:
#  - `all_chars', defined in chars.s

# Provides:
	.globl	main # :: int, [char *], [char *] -> int
	.globl	setUpDisplay # :: int, int -> void
	.globl	showDisplay # :: void -> void
	.globl	delay # :: int -> vovid
	.globl	isUpper # :: char -> int
	.globl	isLower # :: char -> int

	.globl	CHRSIZE
	.globl	NROWS
	.globl	NDCOLS
	.globl	MAXCHARS
	.globl	NSCOLS
	.globl	CLEAR


########################################################################
	.data

	# /!\ NOTE /!\
	# In C, the values of the symbols `CHRSIZE', `NROWS', `NDCOLS',
	# `NSCOLS', `MAXCHARS', and `CLEAR' would be substituted during
	# preprocessing.  SPIM does not have preprocessing facilities,
	# so instead we provide these values in the `.data' segment.

	# # of rows and columns in each big char
CHRSIZE:	.word	9
	# number of rows in all matrices
NROWS:		.word	9
	# number of columns in display matrix
NDCOLS:		.word	80
	# max length of input string
MAXCHARS:	.word	100
	# number of columns in bigString matrix
	# max length of buffer to hold big version
	# the +1 allows for one blank column between letters
NSCOLS:		.word	9000	# (NROWS * MAXCHARS * (CHRSIZE + 1))
        # ANSI escape sequence for 'clear-screen'
CLEAR:	.asciiz "\033[H\033[2J"
# CLEAR:	.asciiz "__showpage__\n" # for debugging

main__0:	.asciiz	"Usage: ./scroll String\n"
main__1:	.asciiz	"Only letters and spaces are allowed in the string!\n"
main__2:	.asciiz "String mush be < "
main__3:	.asciiz " chars\n"
main__4:	.asciiz "Please enter a string with at least one character!\n"

	.align	4
theString:	.space	101	# MAXCHARS + 1
	.align	4
display:	.space	720	# NROWS * NDCOLS
	.align	4
bigString:	.space	81000	# NROWS * NSCOLS
    .align	4
new_line:   .asciiz "\n"
    .align	4
space:      .ascii " "
    .align	4
A:          .ascii "A"
    .align	4
a:          .ascii "a"
    .align	4
twentysix:  .ascii "26"
########################################################################
# .TEXT <main>
	.text
main:

# Frame:	$fp, $ra, $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7
# Uses:		$a0 - #a3, $t0-$t9 $s0 - $s7, 
# Clobbers:	$s0 - $s7

# Locals:
#	- `theLength' in $s0
#	- `bigLength' in $s1
#	- `ch' in $s2
#	- `str' in $t2
#	- `i' in $t3
#	- `j' in $t4
#	- `NROWS' in $t5
#	- `NDCOLS' in $t6
#	- `row' in $t7
#	- `col' in $t8
#	- `iterations' and 'which in $t9
#	- `startingCol' in $s3
#   - address of display in $s4
#   - offset in $s5
#   - address of "theString" in $s6
#   - space is in $s7
#   - CHRSIZE in $a2
#   - address of bigstring in $a3

# Structure:
#	main
#	-> [prologue]
#	-> main_argc_gt_two
#	-> main_PTRs_init
#	  -> main_PTRs_cond
#	    -> main_ch_notspace
#	    -> main_ch_isLower
#	    -> main_ch_isSpace
#	  -> main_PTRs_step
#	-> main_PTRs_f
#	[theLength cond]
#	  | main_theLength_ge_MAXCHARS
#	  | main_theLength_lt_MAXCHARS
#	  | main_theLength_lt_1
#	  | main_theLength_ge_1
#	...
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)  # note: switch to $fp-relative
	sw	$s0, -8($fp)
	sw	$s1, -12($fp)
	sw	$s2, -16($fp)
	sw	$s3, -20($fp)
	sw	$s4, -24($fp)
	sw	$s5, -28($fp)
	sw	$s6, -32($fp)
	sw	$s7, -36($fp)
	
	addi	$sp, $sp, -40

	# if (argc < 2)
	li	$t0, 2
	bge	$a0, $t0, main_argc_gt_two
	nop	# in delay slot
	# printf(...)
	la	$a0, main__0
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_argc_gt_two:

	move	$s0, $zero
main_PTRs_init:
	# s = argv[1]
	lw	$t2, 4($a1)
main_PTRs_cond:
	# optimisation: `ch = *s' now
	# (ch = )*s
	lb	$s2, ($t2)
	# *s != '\0'  =>  ch != 0
	beqz	$s2, main_PTRs_f
	nop	# in delay slot

	# if (!isUpper(ch))
main_ch_upper:
	move	$a0, $s2
	jal	isUpper
	nop	# in delay slot
	beqz	$v0, main_ch_lower
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot
	# if (!isLower(ch))
main_ch_lower:
	move	$a0, $s2
	jal	isLower
	nop	# in delay slot
	beqz	$v0, main_ch_space
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot
	# if (ch != ' ')
main_ch_space:
	li	$t0, ' '
	bne	$s2, $t0, main_ch_fail
	nop	# in delay slot
	j	main_ch_ok
	nop	# in delay slot

main_ch_fail:
	# printf(...)
	la	$a0, main__1
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# exit(1)  =>  return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot

main_ch_ok:
	# if (theLength >= MAXCHARS)
	la	$t0, MAXCHARS
	lw	$t0, ($t0)
	# break  =>  jump out of for(*s...)
	bge	$s0, $t0, main_PTRs_f

	# theString[theLength]
	la	$t0, theString
	addu	$t0, $t0, $s0	# ADDU because address
	# theString[theLength] = ch
	sb	$s2, ($t0)

	# theLength++
	addi	$s0, $s0, 1

main_PTRs_step:
	# s++  =>  s = s + 1
	addiu	$t2, $t2, 1	# ADDIU because address
	j	main_PTRs_cond
	nop
main_PTRs_f:

	# theString[theLength] = ...
	la	$t0, theString
	addu	$t0, $t0, $s0	# ADDU because address
	# theString[theLength] = '\0'
	sb	$zero, ($t0)

	# CHRSIZE + 1
	la	$t0, CHRSIZE
	lw	$t0, ($t0)
	addi	$t0, $t0, 1
	# bigLength = theLength * (CHRSIZE + 1)
	mul	$s1, $t0, $s0

	# if (theLength >= MAXCHARS)
	la	$t0, MAXCHARS
	lw	$t0, ($t0)
	blt	$s0, $t0, main_theLength_lt_MAXCHARS
	nop	# in delay slot
main_theLength_ge_MAXCHARS:
	# printf(..., ..., ...)
	la	$a0, main__2
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	move	$a0, $t0
	li	$v0, 1 # PRINT_INT_SYSCALL
	syscall
	la	$a0, main__3
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_theLength_lt_MAXCHARS:

	# if (theLength < 1)
	li	$t0, 1
	bge	$s0, $t0, main_theLength_ge_1
	nop	# in delay slot
main_theLength_lt_1:
	# printf(...)
	la	$a0, main__4
	li	$v0, 4 # PRINT_STRING_SYSCALL
	syscall
	# exit(1)  =>  return 1  =>  load $v0, jump to epilogue
	li	$v0, 1
	j	main__post
	nop	# in delay slot
main_theLength_ge_1:

#for (i = 0; i < NROWS; i++) {
#      for (j = 0; j < NDCOLS; j++)
#         display[i][j] = ' ';
#   }
    la $t5, NROWS
    lw $t5, ($t5)
    la $t6, NDCOLS
    lw $t6, ($t6)
   # li $t6, 9
  #  li $t6, 80

    li $t3, 0                       #i = 0
main_outer_loop:
    bge $t3, $t5, main_end_outer_loop    # if (i >= NROWS)
    li $t4, 0                       #j = 0
    
main_inner_loop:
    bge $t4, $t6, main_end_inner_loop   #if (j >= NDCOLS)
    #calculate offset
    mul $s5, $t3, $t6   #$s5 = i * NDCOLS///
    add $s5, $s5, $t4   #$s5 = i * NDCOLS + j-> $s5 is the final offset value
#   mul $s5, $t6, $t7   #$s5 = 1(i * NROWS + j) no need to multiply this by 1
    la $s4, display
  #  add $s5, $s4, $s5   #$s5 = address of display + offset
    la $a0, space
    lb $a0, ($a0)
    sb $a0, display($s5)       #display[row][out_col] = ' '
    
    addiu $t4, $t4, 1                #j++
    j main_inner_loop

main_end_inner_loop:
    addiu $t3, $t3, 1                #i++
    j main_outer_loop
    
main_end_outer_loop:

# create the bigchars array
    li $t3, 0
main_create_bc_array:
    bge $t3, $s0, main_create_bc_array_end  #if (i >= theLength) goto end
	la $s6, theString
	add $s5, $s6, $t3   #offset = address of the string + i
	lb $s5, ($s5)       #offset = *thestring[i]
	la $s7, space
	lb $s7, ($s7)       #s7 = space
	bne $s5, $s7, main_ch_ne_space  # if (theString[i] != space) goto else part
    
main_ch_e_space:
    #for (row = 0; row < CHRSIZE; row++) {
    #       for (col = 0; col < CHRSIZE; col++)
    #            bigString[row][col + i * (CHRSIZE+1)] = ' ';
    #     }
    li $t7, 0           #row = 0
main_ch_e_space_ol_start:
    la $a2, CHRSIZE
    lw $a2, ($a2)       #a2 = CHRSIZE
    bge $t7, $a2, main_ch_e_space_ol_end    #if (row >= CHRSIZE) goto end outer loop
    li $t8, 0           #col = 0
main_ch_e_space_il_start:
    bge $t8, $a2, main_ch_e_space_il_end    #if (col >= CHRSIZE) goto end inner loop
    la $a2, CHRSIZE     
    lw $a2, ($a2)       #a2 = CHRSIZE
    lw $a0, NSCOLS
    mul $s5, $t7, $a0   #offset = row * NSCOLS///
    addi $a2, $a2, 1    #chrsize + 1
    mul $a2, $a2, $t3   #i * (CHRSIZE + 1)
    add $a2, $a2, $t8   #col + i * (CHRSIZE+1)
    add $s5, $s5, $a2   #final offset
    la $a3, bigString
    add $s5, $s5, $a3   #bigstring+offset
    
    la $s7, space
    lb $s7, ($s7)       #s7 = space
    sb $s7, ($s5)       #at bigstring at this offset, make it a space
    
    addi $t8, $t8, 1    #col++
    j main_ch_e_space_il_start
    
main_ch_e_space_il_end:
    addi $t7, $t7 1
    j main_ch_e_space_ol_start
main_ch_e_space_ol_end:

main_ch_ne_space:

#else {
#         int which;
#         if (isUpper(ch)) which = ch - 'A'; 
#       if (isLower(ch)) which = ch - 'a' + 26;
#         for (row = 0; row < CHRSIZE; row++) {
#            for (col = 0; col < CHRSIZE; col++) {
#              // copy char to the buffer
#               bigString[row][col + i * (CHRSIZE+1)] = all_chars[which][row][col]; 
#            }
#         }
#      }
    #ch is in $s5
    li $t9, 0               #which = t9 = 0
    move $t4, $s5           #t4 = s5 ->ch is in $t4 now
    move $a0, $t4           #a0 = s5
    jal isUpper
    nop
    
    beqz $v0, main_else_not_upper      # if (isUpper(ch) == 0) goto not upper
    la $t9, A
    lb $t9, ($t9)           #t9 = 'A'
    sub $t4, $t4, $t9
    move $t9, $t4           #which = ch - 'A'
    li $t7, 0               #row = 0
    j main_else_ol

main_else_not_upper:
    jal isLower
    beqz $v0, main_ch_is_space  #if isupper = 0 (not true)
main_else_is_lower:    
    la $t9, a
    lb $t9, ($t9)           #t9 = 'a'
    sub $t4, $t4, $t9
    la $t9, twentysix
    lw $t9, ($t9)           #t9 = '26'
    add $t4, $t4, $t9
    move $t9, $t4           #which = ch - 'a' + 26
    
main_ch_is_space:
    li $t7, 0               #row = 0
main_else_ol:
    la $a2, CHRSIZE
    lw $a2, ($a2)           #$a2 = CHRSIZE
    bge $t7, $a2, main_else_ol_end  #if (row >= CHRSIZE) exit outer loop
    li $t8, 0               #col = 0
main_else_il:
    bge $t8, $a2, main_else_il_end
    #bigString[row][col + i * (CHRSIZE+1)] = all_chars[which][row][col]; 
    mul $s5, $a2, $t9   #offset = which * CHRSIZE
    mul $s5, $s5, $a2   #offset = offset * CHRSIZE
    mul $a0, $t7, $a2   #a0 = row * CHRSIZE
    add $s5, $a0, $s5   #offset = offset+a0
    add $s5, $s5, $t8   #offset + col
    la $a0, all_chars
   # add $a0, $s5, $a0   #all_chars + offset
    
    lw $a2, CHRSIZE
    addi $s5, $a2, 1    #offset = CHRSIZE + 1
    mul $s5, $s5, $t3   #offset = offset * i
    add $s5, $s5, $t8   #offset = offset + col
    lw $a2, NSCOLS
    mul $a2, $t7, $a2   #a2 = row * NSCOLS ///
    add $s5, $s5, $a2   #a2+ offset
    la $a3, bigString
    add $a3, $a3, $s5   #bigString+ offset
    
    lb $a0, ($a0)
    sb $a0, ($a3)
   
    addi $t8, $t8 1     #col++
    j main_else_il
    
main_else_il_end:
    addi $t7, $t7 1     #row++
    j main_else_ol
    
main_else_ol_end:

main_end_if_else:

#col = (i * (CHRSIZE+1)) + CHRSIZE;
#      for (row = 0; row < CHRSIZE; row++)
#         bigString[row][col] = ' ';
     
    la $a2, CHRSIZE   
    lw $a2, ($a2)           #a2 = CHRSIZE   
    addi $t8, $a2, 1        #col = CHRSIZE+1  
    mul $t8, $t8, $t3       #col = CHRSIZE+1 * i
    add $t8, $t8, $a2       #col = col + CHRSIZE
   
    li $t7, 0               #row = 0
main_col_loop:
    bge $t7, $a2, main_col_loop_end #if (row >= CHRSIZE) goto end
    la $a3, bigString   #a3 = address of bigstring
    lw $a0, NSCOLS
    mul $s5, $t7, $a0   #$s5 = row * NSCOLS
    add $s5, $s5, $t8   #$s5 = row * NSCOLS + col-> $s5 is the final offset value
    add $a3, $a3, $s5   #$s5 = address of bigString + offset  
    la $a0, space
    lb $a0, ($a0)
    sb $a0, ($a3)
    
    addi $t7, $t7, 1    #row++
    j main_col_loop
main_col_loop_end:    
    
    addi $t3, $t3, 1
    j main_create_bc_array
    
main_create_bc_array_end:
# for (i = 0; i < iterations; i++) {
#    setUpDisplay(starting_col, bigLength);
#   showDisplay();
# starting_col--;
#   delay(1);
#  }

    add $t9, $t6, $s1   #iterations = NDCOLS+bigLength
    subu $s3, $t6, 1    #starting_col = NDCOLS-1
    
    li $t3, 0       # i = 0
main_last_loop:    
    bge $t3, $t9, main_last_loop_end    #if(i >= iterations) goto end
	move $a0, $s3       #argument1 = starting_col
	move $a1, $s1       #argument2 = bigLength
	jal setUpDisplay
	nop
	jal showDisplay
	nop
	subu $s3, $s3, 1    #starting_col--
	li $a0, 1           #a0 = 1
	#jal delay
	addi $t3, $t3, 1
	j main_last_loop
	
main_last_loop_end:	
		
	# return 0
	move	$v0, $zero
main__post:
	# tear down stack frame
	
	
	lw	$s2, -36($fp)
	lw	$s2, -32($fp)
	lw	$s2, -28($fp)
	lw	$s2, -24($fp)
	lw	$s2, -20($fp)
	lw	$s2, -16($fp)
	lw	$s1, -12($fp)
	lw	$s0, -8($fp)
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <setUpDisplay>
	.text
setUpDisplay:

# Frame:	$fp, $ra
# Uses:		$a0, $a1, $t0 - $t9

# Locals:
#	- `row' in $t0
#	- `out_col' in $t1
#	- `in_col' in $t2
#	- `first_col' in $t3
#	- -1 and address of bigStringin $t4
#   - NROWS in $t5
#   - NDCOLS in $t6
#   - address of bigString in $t7
#   - offset in $t8
#   - address of display in $t9

# Structure:
#	setUpDisplay
#	-> [prologue]
#	-> sud_starting_lt_zero
#	-> sud_starting_ge_zero
#	    -> sud_outer_loop
#	        -> sud_inner_loop
#	        -> sud_end_inner_loop
#	    -> sud_end_outer_loop
#       -> sud_second_outer_loop
#	        -> sud_second_inner_loop
#	        -> sud_second_inner_loop_end
#	    -> sud_second_outer_loop_end
#	-> sud_setUpDisplay_end
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# ... TODO ...
	#if (starting < 0) {
    #  // start part-way through bigString
   #   // it's scrolling off the left of the display
  #    out_col = 0;
  #    first_col = -starting;
 #  }
 #  else {
 #     for (out_col = 0; out_col < starting; out_col++) {
 #        for (row = 0; row < NROWS; row++)
 #           display[row][out_col] = ' ';
#      }
  #    first_col = 0;
 #  }
 #  // copy the relevant bits of the bigString into the display
#   for (in_col = first_col; in_col < length; in_col++) {
#      if (out_col >= NDCOLS) break;
 #     for (row = 0; row < NROWS; row++) {
  #       display[row][out_col] = bigString[row][in_col];
 #     }
  #    out_col++;
  # }
#}
    #$a0 = starting
    #$a1 = length
    #initialising variables
    la $t5, NROWS
    lw $t5 ($t5)                   #$t5 = NROWS
    la $t6, NDCOLS  
    lw $t6, ($t6)                  #$t6 = NDCOLS
    la $t9, display
    la $t7, bigString
    
    bge $a0, $0, sud_starting_ge_zero   #if (staring >= 0)
    
sud_starting_lt_zero:
    li $t1, 0                       #out_col = 0
    li, $t4, -1                     # $t4 = -1
    mul $t3, $t4, $a0               # first_col = -1 * starting
    j sud_end_if_else
    
sud_starting_ge_zero:
    li $t1, 0                       #out_col = 0    
sud_outer_loop:
    bge $t1, $a0, sud_end_outer_loop    # if (out_col >= starting)
    li $t0, 0                       #row = 0
    
sud_inner_loop:
    bge $t0, $t5, sud_end_inner_loop
    #display[row][out_col] = ' ';
    #calculate offset
    mul $t8, $t0, $t6   #$t8 = row * NDCOLS
    add $t8, $t8, $t1   #$t8 = row * NDCOLS + out_col-> $t8 is the final offset value
#   mul $t8, $t6, $t7   #$t8 = 1(i * NROWS + j) no need to multiply this by 1
    add $t8, $t9, $t8   #$t8 = address of display + offset
    la $a2, space
    lb $a2, ($a2)
    sb $a2, ($t8)       #display[row][out_col] = ' '
    
    addi $t0, $t0, 1                #row++
    j sud_inner_loop

sud_end_inner_loop:
    addi $t1, $t1, 1                #outcol++
    j sud_outer_loop
sud_end_outer_loop:
sud_end_if_else:
    li $t3, 0                       #first_col = 0;
    move $t2, $t3                   #in_col = first_col
    
sud_second_outer_loop:
    bge $t2, $a1, sud_setUpDisplay_end  #if (in_col >= length), goto end
    bge $t1, $t6, sud_setUpDisplay_end  #if (out_col >= NDCOLS), goto end
    
    li, $t0, 0                      #row = 0
sud_second_inner_loop:
    la $t5, NROWS
    lw $t5, ($t5)
    bge $t0, $t5, sud_second_inner_loop_end  # if (row >= NROWS)
    #display[row][out_col] = bigString[row][in_col];
    mul $t8, $t0, $t6   #$t8 = row * NDCOLS
    add $t8, $t8, $t1   #$t8 = row * NDCOLS + out_col-> $t8 is the final offset value
    la $t9, display
    add $t9, $t9, $t8   #$t9 = address of display + offset
    
    la $t4, bigString   #t4 = address of bigstring
    lw $a3, NSCOLS
    mul $t8, $t0, $a3   #offfset = row * NSCOLS
    add $t8, $t8, $t2   #offset + in_col
    add $t4, $t8, $t4   #$t4 = address of bigString + offset
    lb $t4, ($t4)       #$t4 = contents of bigstring at that offset
    sb $t4, ($t9)       #display[row][out_col] = bigString[row][in_col];
                        #sw	Rs, Addr	Mem[Addr] = Rs
    
    addi $t0, $t0, 1                     #row++
    j sud_second_inner_loop
    
sud_second_inner_loop_end:
    addi $t2, $t2, 1                     #in_col++
    addi $t1, $t1, 1                     #out_col++
    j sud_second_outer_loop

sud_setUpDisplay_end:   
    
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <showDisplay>
	.text
showDisplay:

# Frame:	$fp, $ra, ...
# Uses:		$a0, $v0, $t0 - $t6

# Locals:
#	- `i' in $t0
#	- NROWS in $t1
#	- `j' in $t2
#	- NDCOLS in $t3
#   - address of display in $t4
#   - size of int (4) in $t5
#   - offset stored in $t6

# Structure:
#	showDisplay
#	-> [prologue]
#	-> showDisplay_outer_loop
#	    -> showDisplay_inner_loop
#	    ->showDisplay_end_of_inner_loop
#	-> showDisplay_end_of_outer_loop
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	#printf(CLEAR);
   #for (int i = 0; i < NROWS; i++) {
     # for (int j = 0; j < NDCOLS; j++)
        # putchar(display[i][j]);
    #  putchar('\n');
  # }
    #printf(CLEAR)
    la $a0, CLEAR
    lw $a0, ($a0)
    li $v0, 4
    syscall
    
    li $t0, 0 #i = 0
    la $t1, NROWS
    lw $t1, ($t1)
    la $t3, NDCOLS
    lw $t3, ($t3)
showDisplay_outer_loop:
    bge $t0, $t1, showDisplay_end_of_outer_loop #if (i >= NROWS)
    li $t2, 0 #j = 0

showDisplay_inner_loop:       
    bge $t2, $t3, showDisplay_end_of_inner_loop #if (j >= NDCOLS)
    #print display[i][j]
    la $t4, display
    #calculate offset
    mul $t6, $t0, $t3   #$t6 = i * NDCOLS
    add $t6, $t6, $t2   #$t6 = i * NDCOLS + j
  #  add $t6, $t6, $t4   #$t6 = address of display + offset
    #print
    lb $a0, display($t6)
    li $v0, 11
    syscall    
    
    addiu $t2, $t2, 1 #j++
    j showDisplay_inner_loop    

showDisplay_end_of_inner_loop:         
    #print \n
    la $a0, new_line
    lw $a0, ($a0)
    li $v0, 4
    syscall    
    addiu $t0, $t0, 1 #i++
    j showDisplay_outer_loop    

showDisplay_end_of_outer_loop:    

	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <delay>
	.text
delay:

# Frame:	$fp, $ra
# Uses:		$a0, $t0, $t1, $t2, $t3, $t4, $t5
# Clobbers:	$t0, $t1, $t2, $t3, $t4, $t5

# Locals:
#	- `n' in $a0
#	- `x' in $t0
#	- `i' in $t1
#	- `j' in $t2
#	- `k' in $t3

# Structure:
#	delay
#	-> [prologue]
#	-> delay_i_init
#	-> delay_i_cond
#	   -> delay_j_init
#	   -> delay_j_cond
#	      -> delay_k_init
#	      -> delay_k_cond
#	         -> delay_k_step
#	      -> delay_k_f
#	      -> delay_j_step
#	   -> delay_j_f
#	   -> delay_i_step
#	-> delay_i_f
#	-> [epilogue]

# Code:
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# x <- 0
	move	$t0, $zero
	# These values control the busy-wait.
	li	$t4, 20000
	li	$t5, 1000

delay_i_init:
	# i = 0;
	move	$t1, $zero
delay_i_cond:
	# i < n;
	bge	$t1, $a0, delay_i_f
	nop	# in delay slot

delay_j_init:
	# j = 0;
	move	$t2, $zero
delay_j_cond:
	# j < DELAY_J;
	bge	$t2, $t4, delay_j_f
	nop	# in delay slot

delay_k_init:
	# k = 0;
	move	$t3, $zero
delay_k_cond:
	# k < DELAY_K;
	bge	$t3, $t5, delay_k_f
	nop	# in delay slot

	# x = x + 1
	addi	$t0, $t0, 1

delay_k_step:
	# k = k + 1
	addi	$t3, $t3, 1
	j	delay_k_cond
	nop	# in delay slot
delay_k_f:

delay_j_step:
	# j = j + 1
	addi	$t2, $t2, 1
	j	delay_j_cond
	nop	# in delay slot
delay_j_f:

delay_i_step:
	# i = i + 1
	addi	$t1, $t1, 1
	j	delay_i_cond
	nop	# in delay slot
delay_i_f:

delay__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <isUpper>
	.text
isUpper:

# Frame:	$fp, $ra
# Uses:		$a0
# Clobbers:	$v0

# Locals:
#	- 'ch' is in $a0
#   - $v0 is used as a temporary register

# Structure:
#	isUpper
#	-> [prologue]
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)
	
	# if (ch < 'A')
	li  $v0, 'A'
	blt $a0, $v0 isUpper_ch_lt_A
	# if (ch > 'Z')
	li $v0, 'Z'
	bgt $a0, $v0 isUpper_ch_gt_Z
	#ch must be between 'A' and 'Z'
	li, $v0, 1
	j isUpper_ch_between_AZ
	
isUpper_ch_lt_A:	
isUpper_ch_gt_Z:

    li $v0, 0	
	
isUpper_ch_between_AZ:

	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

########################################################################
# .TEXT <isLower>
	.text
isLower:

# Frame:	$fp, $ra
# Uses:		$a0
# Clobbers:	$v0

# Locals:
#	- `ch' in $a0
#	- ... $v0 used as temporary register

# Structure:
#	isLower
#	-> [prologue]
#	[ch cond]
#	   | isLower_ch_ge_a
#	   | isLower_ch_le_z
#	   | isLower_ch_lt_a
#	   | isLower_ch_gt_z
#	-> isLower_ch_phi
#	-> [epilogue]

# Code:
	# set up stack frame
	sw	$fp, -4($sp)
	la	$fp, -4($sp)
	sw	$ra, -4($fp)
	la	$sp, -8($fp)

	# if (ch >= 'a')
	li	$v0, 'a'
	blt	$a0, $v0, isLower_ch_lt_a
	nop	# in delay slot
isLower_ch_ge_a:
	# if (ch <= 'z')
	li	$v0, 'z'
	bgt	$a0, $v0, isLower_ch_gt_z
	nop	# in delay slot
isLower_ch_le_z:
	addi	$v0, $zero, 1
	j	isLower_ch_phi
	nop	# in delay slot

	# ... else
isLower_ch_lt_a:
isLower_ch_gt_z:
	move	$v0, $zero
	# fallthrough
isLower_ch_phi:

isLower__post:
	# tear down stack frame
	lw	$ra, -4($fp)
	la	$sp, 4($fp)
	lw	$fp, ($fp)
	jr	$ra
	nop	# in delay slot

#################################################################### EOF
