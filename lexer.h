#ifndef LEXER_H
#define LEXER_H

// Token types
typedef enum {
    T_KEYWORD,
    T_IDENTIFIER,
    T_NUMBER,
    T_OPERATOR,
    T_PUNCTUATION,
    T_TYPE,
    T_POINTER_TYPE,
    T_DOUBLE_COLON,
    T_UNKNOWN
} TokenType;

const char* token_type_to_str(TokenType type);

// Structure to hold token information
typedef struct {
    TokenType type;
    char* value;
    size_t line;
    size_t column;
    char* filename;
} Token;

// Function declarations
Token** tokenize(const char* input_str, size_t* token_count);
void tokenize_line(const char* line, size_t line_number, const char* filename, Token*** buffer, size_t* token_count);
void print_tokens(Token** tokens, size_t token_count, TokenType* token_type);
void free_tokens(Token** tokens, size_t token_count);

#endif // LEXER_H
