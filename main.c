#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_file_fseek(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for string (+1 for null terminator)
    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    // Read file content
    size_t read_size = fread(buffer, 1, file_size, file);
    buffer[read_size] = '\0';  // Add null terminator

    fclose(file);
    return buffer;
}

int main() {
    const char* code = read_file_fseek("example.ngp");

    size_t token_count;
    Token** token_arr = tokenize(code, &token_count);

    print_tokens(token_arr, token_count);
    free_tokens(token_arr, token_count);

    return 0;
}
