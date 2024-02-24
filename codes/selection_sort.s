.data 
base: .word 3 2 1 5 6 9 90
.text
la x1 base
addi x2 x0 0
addi x2 x2 3
addi x16 x0 4
sort:
addi x3 x0 0
addi x4 x0 0
addi x5 x0 0
loop1:
addi x6 x2 -1
bge x3 x6 el1
add x5 x0 x3
addi x4 x3 1
loop2:
bge x4 x2 i2
mul x7 x4 x16
add x8 x1 x7
lw x9 0(x8)
mul x10 x5 x16
add x11 x1 x10
lw x12 0(x11)
i1:
bge x9 x12 ei1
add x5 x0 x4
addi x4 x4 1
j loop2
i2:
beq x5 x3 ei2
mul x7 x3 x16
add x8 x1 x7
lw x9 0(x8)
mul x10 x5 x16
add x11 x1 x10
lw x12 0(x11)
sw x9 0(x11)
sw x12 0(x8)
addi x3 x3 1
j loop1
ei2:
addi x3 x3 1
j loop1
ei1:
addi x4 x4 1
j loop2
el1:
add x0 x0 x0