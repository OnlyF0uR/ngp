CC = clang
CFLAGS = -Wall -std=c18

OBJFILES = utils.o lexer.o parser.o ast.o main.o
EXEC = ngp.exe

# Build the final executable
$(EXEC): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJFILES)

# Compile utils.c
utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

# Compile lexer.c
lexer.o: lexer.c lexer.h
	$(CC) $(CFLAGS) -c lexer.c

# Compile parser.c
parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

# Compile ast.c
ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

# Compile main.c
main.o: main.c lexer.h
	$(CC) $(CFLAGS) -c main.c

# Clean the project
clean:
	rm -f $(OBJFILES) $(EXEC)
