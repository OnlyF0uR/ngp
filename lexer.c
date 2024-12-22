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
    // Find the length to duplicate, the smaller of n or the string's length
    size_t len = 0;
    while (len < n && str[len] != '\0') {
        len++;
    }

    // Allocate memory for the new string (+1 for null terminator)
    char* result = (char*)malloc(len + 1);
    if (!result) {
        return NULL; // Return NULL if memory allocation fails
    }

    // Copy up to len characters
    for (size_t i = 0; i < len; i++) {
        result[i] = str[i];
    }

    // Null-terminate the string
    result[len] = '\0';

    return result;
}

Token* make_token(TokenType type, const char* value) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = _strdup(value);

    return token;
}

void free_token(Token* token) {
    if (token) {
        free(token->value);
        free(token);
    }
}

void add_token(Token*** tokens, size_t* token_count, TokenType type, const char* value) {
    Token* token = make_token(type, value);
    *tokens = (Token**)realloc(*tokens, (*token_count + 1) * sizeof(Token*));
    (*tokens)[*token_count] = token;
    (*token_count)++;
}

Token** tokenize(const char* input_str, size_t* token_count) {
    *token_count = 0;
    Token** tokens = NULL;

    size_t len = strlen(input_str);
    for (size_t i = 0; i < len; i++) {
        if (isspace(input_str[i])) {
            continue; // Skip whitespace
        }

        // TODO: Add all tokens
        if (isalpha(input_str[i])) {
            // Parse an identifier
            size_t start = i;
            while (i < len && (isalnum(input_str[i]) || input_str[i] == '_')) {
                i++;
            }
            char* identifier = strndup(input_str + start, i - start);

            // So if this is pub, fn, i32, u8, or return, it's a keyword
            if (strcmp(identifier, "pub") == 0 ||
                strcmp(identifier, "fn") == 0 ||
                strcmp(identifier, "return") == 0
            ) {
                add_token(&tokens, token_count, T_KEYWORD, identifier);
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
                add_token(&tokens, token_count, T_TYPE, identifier);
            } else {
                add_token(&tokens, token_count, T_IDENTIFIER, identifier);
            }

            free(identifier);
            i--; // Adjust for the loop increment
        } else if (isdigit(input_str[i])) {
            // Parse a number
            size_t start = i;
            while (i < len && isdigit(input_str[i])) {
                i++;
            }
            char* number = strndup(input_str + start, i - start);
            add_token(&tokens, token_count, T_NUMBER, number);
            free(number);
            i--; // Adjust for the loop increment
        } else if (strchr("+-*/=", input_str[i])) {
            // Parse an operator
            char operator[2] = {input_str[i], '\0'};
            add_token(&tokens, token_count, T_OPERATOR, operator);
        } else {
            // Unknown token
            char unknown[2] = {input_str[i], '\0'};
            add_token(&tokens, token_count, T_UNKNOWN, unknown);
        }
    }

    return tokens;
}

void free_tokens(Token** tokens, size_t token_count) {
    for (size_t i = 0; i < token_count; i++) {
        free_token(tokens[i]);
    }
    free(tokens);
}

void print_tokens(Token** tokens, size_t token_count) {
    for (size_t i = 0; i < token_count; i++) {
        printf("Token Type: %s, Value: '%s'\n", token_type_to_str(tokens[i]->type), tokens[i]->value);
    }
}
