CC = clang
CFLAGS = -Wall -std=c11

OBJFILES = lexer.o parser.o main.o
EXEC = ngpcomp.exe

# Build the final executable
$(EXEC): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJFILES)

# Compile lexer.c
lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

# Compile parser.c
parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

# Compile main.c
main.o: main.c lexer.h
	$(CC) $(CFLAGS) -c main.c

# Clean the project
clean:
	rm -f $(OBJFILES) $(EXEC)
