#ifndef LEXER_H
#define LEXER_H

// Token types
typedef enum {
    T_KEYWORD, // 1
    T_IDENTIFIER, // 2
    T_NUMBER, // 3
    T_OPERATOR,  // 4
    T_PUNCTUATION, // 5
    T_TYPE, // 6
    T_UNKNOWN, // 7
    T_END // 8
} TokenType;

const char* token_type_to_str(TokenType type);

// Structure to hold token information
typedef struct {
    TokenType type;
    char* value;
} Token;

// Function declarations
Token* create_token(TokenType type, const char* value);
void free_token(Token* token);
Token** tokenize(const char* input_str, size_t* token_count);
void print_tokens(Token** tokens, size_t token_count);
void free_tokens(Token** tokens, size_t token_count);

#endif // LEXER_H