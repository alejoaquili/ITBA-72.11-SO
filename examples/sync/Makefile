IMP=ERROR

all:
	nasm -g -felf64 atomic.asm
	gcc -Wall -g atomic.o sem.c main.c -DIMP_$(IMP) -lpthread
