.data
base: .word 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1
.text
addi x1 x0 0 #0
addi x2 x0 0 #1
addi x3 x0 18 #2
addi x19 x0 19 # 3
addi x18 x0 20 #4 
la x16 base #5
la x17 base #6
la x21 base #7
la x20 base #8
loop:
addi x2 x1 1 #9
addi x16 x20 0 #10
addi x15 x20 4 #11
loop1:
beq x18 x2 exit #12
lw x5 0(x16) #13
lw x6 0(x15) #14
blt x5 x6 exit2 #15
sw x5 0(x15) #16
sw x6 0(x16) #17
exit2:
addi x15 x15 4 #18
addi x2 x2 1 #19
j loop1 #20
exit:
addi x1 x1 1 #21
addi x20 x20 4 #22
bne x1 x19 loop #23
add x0 x0 x0 #24