.data
arr: .word 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1
##
.text
la x5 arr
add x6 x5 x0 # copy to go back for after iteration
addi x7 x0 20 # x7 is decremented in each iteration
addi x8 x0 20 # saves the length of array and is not modified
loop1:
beq x0 x7 exit
addi x7 x7 -1
loop2:
beq x1 x8 exit2
addi x1 x1 1
lw x4 0(x5)
lw x9 4(x5)
blt x4 x9 noswap 
sw x4 4(x5)
sw x9 0(x5)
noswap:
addi x5 x5 4
j loop2
exit2:
add x1 x0 x0
add x5 x6 x0
j loop1
exit:
add x0 x0 x0