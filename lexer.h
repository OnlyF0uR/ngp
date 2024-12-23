#ifndef LEXER_H
#define LEXER_H

// Token types
typedef enum {
    T_KEYWORD,
    T_IDENTIFIER,
    T_NUMBER,
    T_OPERATOR,
    T_TYPE,
    T_POINTER_TYPE,
    T_DOUBLE_COLON,
    T_L_PAREN,
    T_R_PAREN,
    T_L_BRACE,
    T_R_BRACE,
    T_L_BRACKET,
    T_R_BRACKET,
    T_STRING,
    T_SEMICOLON,
    T_COMMENT,
    T_COMMA,
    T_L_ANGLE_BRACKET,
    T_R_ANGLE_BRACKET,
    T_MACRO_CALL,
    T_DOT,
    T_EQUAL_SIGN,
    T_EXCLAMATION_MARK,
    T_NOT_EQUAL_SIGN,
    T_COLON,
    T_HASH_SIGN,
    T_IMPORT,
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
