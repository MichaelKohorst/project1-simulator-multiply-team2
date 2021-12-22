	lw 1 0 mcand
	lw 2 0 mplier
	lw 3 0 one
start	lw 6 0 full
	beq 5 6 done
	lw 6 0 nOne
	nand 4 2 6
	nand 4 4 6
	beq 1 4 mult
increm	add 3 3 3
	add 1 1 1
	lw  4 0 one
	add 5 4 5
	beq 0 0 start
mult	add 7 7 1
secWh   beq 0 0 increm
done    halt
one 	.fill 1
full 	.fill 32
mcand   .fill 29562
mplier  .fill 11834
nOne	.fill -1