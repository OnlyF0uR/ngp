#include "ast.h"
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "lexer.h"
#include "parser.h"
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

    // Handle file reading and lexer
    handle_file_read("example.ngc", &buffer, &token_count);
    // print_tokens(buffer, token_count, NULL);

    // Pass the tokens to the parser
    Parser* parser = create_parser(buffer, token_count);
    run_parser(parser);

    print_ast_node(parser->ast_root, 2);

    // Free everything
    free_parser(parser);
    free_tokens(buffer, token_count);

    return 0;
}
