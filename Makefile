CC=clang
LINK=clang
CFLAGS=-c -O2 -Wall
SHMK=shmk.exe

all : $(SHMK)

$(SHMK) : main.o
	$(LINK) -o $(SHMK) main.o

main.o : src/main.c
	$(CC) $(CFLAGS) src/main.c