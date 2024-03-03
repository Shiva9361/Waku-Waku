.data 
m: .word 5
.text
la x1 m 
lw x2 0(x1)
addi x21 x21 5
loop:
beq x21 x0 exit
addi x21 x21 -1
beq x0 x0 loop
exit:
add x0 x0 x0
