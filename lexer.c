#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const char* token_type_to_str(TokenType type) {
    switch (type) {
        case T_KEYWORD: return "Keyword";
        case T_IDENTIFIER: return "Identifier";
        case T_NUMBER: return "Number";
        case T_OPERATOR: return "Operator";
        case T_PUNCTUATION: return "Punctuation";
        case T_TYPE: return "Type";
        case T_UNKNOWN: return "Unknown";
        case T_END: return "End";
        default: return "Invalid";
    }
}

char* strndup(const char* str, size_t n) {
    size_t len = 0;
    while (len < n && str[len] != '\0') {
        len++;
    }

    char* result = (char*)malloc(len + 1);
    if (!result) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        result[i] = str[i];
    }

    result[len] = '\0';
    return result;
}

Token* make_token(TokenType type, const char* value, size_t line, size_t column, const char* filename) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = _strdup(value);
    token->line = line;
    token->column = column;
    token->filename = _strdup(filename);
    return token;
}

void free_token(Token* token) {
    if (token) {
        free(token->value);
        free(token->filename);
        free(token);
    }
}

void add_token(Token*** tokens, size_t* token_count, TokenType type, const char* value, size_t line, size_t column, const char* filename) {
    Token* token = make_token(type, value, line, column, filename);
    *tokens = (Token**)realloc(*tokens, (*token_count + 1) * sizeof(Token*));
    (*tokens)[*token_count] = token;
    (*token_count)++;

    // printf("Token Type: %s, Value: '%s', Line: %zu, Column: %zu, Filename: %s\n",
    //        token_type_to_str(type), value, line, column, filename);
}

void tokenize_line(const char* line, size_t line_number, const char* filename, Token*** buffer, size_t* token_count) {
    if (line == NULL) {
        printf("Line is NULL\n");
        return;
    }

    if (filename == NULL) {
        printf("Filename is NULL\n");
        return;
    }

    if (*buffer == NULL) {
        *buffer = (Token**)malloc(sizeof(Token*));
        *token_count = 0;
    }

    size_t len = strlen(line);
    size_t column = 1;
    for (size_t i = 0; i < len; i++) {
        column++;
        if (isspace(line[i])) {
            continue;
        }

        if (isalpha(line[i])) {
            size_t start = i;
            while (i < len && (isalnum(line[i]) || line[i] == '_')) {
                i++;
            }
            char* identifier = strndup(line + start, i - start);
            if (identifier == NULL) {
                continue;
            }

            if (strcmp(identifier, "pub") == 0 ||
                strcmp(identifier, "fn") == 0 ||
                strcmp(identifier, "return") == 0
            ) {
                add_token(buffer, token_count, T_KEYWORD, identifier, line_number, column, filename);
            } else if (strcmp(identifier, "u8") == 0 ||
                strcmp(identifier, "u16") == 0 ||
                strcmp(identifier, "u32") == 0 ||
                strcmp(identifier, "u64") == 0 ||
                strcmp(identifier, "u128") == 0 ||
                strcmp(identifier, "i8") == 0 ||
                strcmp(identifier, "i16") == 0 ||
                strcmp(identifier, "i32") == 0 ||
                strcmp(identifier, "i64") == 0 ||
                strcmp(identifier, "i128") == 0 ||
                strcmp(identifier, "f32") == 0 ||
                strcmp(identifier, "f64") == 0 ||
                strcmp(identifier, "bool") == 0
            ) {
                add_token(buffer, token_count, T_TYPE, identifier, line_number, column, filename);
            } else {
                add_token(buffer, token_count, T_IDENTIFIER, identifier, line_number, column, filename);
            }

            free(identifier);
            i--;
        } else if (isdigit(line[i])) {
            size_t start = i;
            while (i < len && isdigit(line[i])) {
                i++;
            }

            char* number = strndup(line + start, i - start);
            if (number == NULL) {
                i--;
                continue;
            }

            add_token(buffer, token_count, T_NUMBER, number, line_number, column, filename);
            free(number);
            i--;
        } else if (strchr("+-*/=", line[i])) {
            if (line[i] == '/' && i + 1 < len && line[i + 1] == '/') {
                while (i < len && line[i] != '\n') {
                    i++;
                }
                i--;
            } else {
                char operator[2] = {line[i], '\0'};
                add_token(buffer, token_count, T_OPERATOR, operator, line_number, column, filename);
            }
        } else if (strchr("{}()[];,", line[i])) {
           char punct[2] = {line[i], '\0'};
           add_token(buffer, token_count, T_PUNCTUATION, punct, line_number, column, filename);
        } else {
            char unknown[2] = {line[i], '\0'};
            add_token(buffer, token_count, T_UNKNOWN, unknown, line_number, column, filename);
        }
    }
}

void free_tokens(Token** tokens, size_t token_count) {
    for (size_t i = 0; i < token_count; i++) {
        free_token(tokens[i]);
    }
    free(tokens);
}

void print_token(Token* token) {
    printf("Token Type: %s, Value: '%s', Line: %zu, Column: %zu, Filename: %s\n",
           token_type_to_str(token->type), token->value, token->line, token->column, token->filename);
}

void print_tokens(Token** tokens, size_t token_count, TokenType* token_type) {
    for (size_t i = 0; i < token_count; i++) {
        if (token_type != NULL && tokens[i]->type != *token_type) {
            continue;
        }
        print_token(tokens[i]);
    }
}
