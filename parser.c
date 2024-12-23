#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

Parser* create_parser(Token** tokens, size_t token_count) {
    Parser* parser = malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->token_count = token_count;
    parser->current = 0;
    return parser;
}

void free_parser(Parser* parser) {
    // NOTE: Tokens should be freed seperately
    // see lexer.h free_tokens function
    free(parser->tokens);
}

Token* current_token(Parser* parser) {
    if (parser->current >= parser->token_count) return NULL;
    return parser->tokens[parser->current];
}

Token* match(Parser* parser, TokenType type) {
    if (parser->current < parser->token_count && parser->tokens[parser->current]->type == type) {
        return parser->tokens[parser->current++];
    }
    return NULL;
}

void error(Parser* parser, const char* message) {
    Token* token = current_token(parser);
    if (token) {
        fprintf(stderr, "Error at %s:%zu:%zu: %s (got %s)\n",
                token->filename, token->line, token->column,
                message, token_type_to_str(token->type));
    } else {
        fprintf(stderr, "Error: %s at end of input\n", message);
    }
    exit(1);
}

void run_parser(Parser* parser) {
    while(parser->current < parser->token_count) {
        // TODO: Implement the parser functionality

        // Move to the next token
        parser->current++;
    }
}
