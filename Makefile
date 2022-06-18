CC=clang
LINK=clang
CFLAGS=-c -O2 -std=c++17
SHMK=build/shmk.exe

all : $(SHMK)

$(SHMK) : main.o
	$(LINK) -o $(SHMK) main.o

main.o : src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp