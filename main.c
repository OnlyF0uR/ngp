#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_file_read(const char* filename, Token*** tokens, size_t* token_count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return;
    }

    char buffer[1024];
    int line_number = 1;

    while (fgets(buffer, sizeof(buffer), file)) {
        // Remove trailing newline if present
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }

        tokenize_line(buffer, line_number, filename, tokens, token_count);
        line_number++;
    }

    fclose(file);
}

int main() {
    Token** buffer = NULL;
    size_t token_count;

    handle_file_read("example.ngc", &buffer, &token_count);

    // TokenType token_type = T_UNKNOWN;
    print_tokens(buffer, token_count, NULL);

    free_tokens(buffer, token_count);

    return 0;
}
