.data
base: .word 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1
.text
addi x1 x0 0
addi x2 x0 0
addi x3 x0 18
addi x19 x0 19
addi x18 x0 20
la x16 base
la x17 base
la x21 base
la x20 base
loop:
addi x2 x1 1
addi x16 x20 0
addi x15 x20 4
loop1:
beq x18 x2 exit
lw x5 0(x16)
lw x6 0(x15)
blt x5 x6 exit2
sw x5 0(x15)
sw x6 0(x16)
exit2:
addi x15 x15 4
addi x2 x2 1
beq x0 x0 loop1
exit:
addi x1 x1 1
addi x20 x20 4
bne x1 x19 loop
add x0 x0 x0