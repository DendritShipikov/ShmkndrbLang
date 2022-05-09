CC=clang
CFLAGS= -c -O2 -Wall
SOURCE_PATH=./src
SHMK=build/shmk.exe
LINKER_FILE_SET= main.o ast.o object.o memory.o parser.o compiler.o

all: $(SHMK)

$(SHMK): $(LINKER_FILE_SET)
	$(CC) -o $(SHMK) $(LINKER_FILE_SET)

$(LINKER_FILE_SET) : $(SOURCE_PATH)/$(*B).c
	$(CC) $(CFLAGS) $(SOURCE_PATH)/$(*B).c