#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct {
    Token** tokens;
    size_t token_count;
    size_t current;
    ASTNode* ast_root;
} Parser;

Parser* create_parser(Token** tokens, size_t token_count);
void run_parser(Parser* parser);
void free_parser(Parser* parser);

#endif // PARSER_H
