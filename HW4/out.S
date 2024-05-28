jal x1, MAIN
a:
data 0 * 1
i:
data 0 * 1
MAIN:
li x1, 0
sw x0, 2, x1
WHILE0:
lw x2, x0, 2
li x3, 5
slt x2, x2, x3
beq x2, x0, ENDWHILE0
lw x1, x0, 2
li x2, 2
add x1, x1, x2
sw x0, 1, x1
lw x1, x0, 2
li x2, 1
add x1, x1, x2
sw x0, 2, x1
jal x1, WHILE0
ENDWHILE0:
ebreak
