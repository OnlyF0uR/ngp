#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "parser.h"
#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char* name;
    char* type;
} Parameter;

Parser* create_parser(Token** tokens, size_t token_count) {
    Parser* parser = malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->token_count = token_count;
    parser->current = 0;
    parser->ast_root = NULL;
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

ASTNode* parse_statement(Parser* parser) {
    // Token* token = parser->tokens[parser->current];

    // TODO: This
    parser->current++;

    return NULL;
}

void run_parser(Parser* parser) {
    ASTNode** statements = malloc(sizeof(ASTNode*) * parser->token_count);
    size_t count = 0;

    while (parser->current < parser->token_count) {
        ASTNode* statement = parse_statement(parser);
        if (statement) {
            statements[count++] = statement;
        }
    }

    if (count > 0) {
        parser->ast_root = create_block_node(statements, count);
    }
}
