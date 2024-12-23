#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TYPE_WIDTH 15
#define VALUE_WIDTH 20
#define LINE_WIDTH 6
#define COL_WIDTH 6
#define FILENAME_WIDTH 20

const char* token_type_to_str(TokenType type) {
    switch (type) {
        case T_KEYWORD: return "Keyword";
        case T_IDENTIFIER: return "Identifier";
        case T_NUMBER: return "Number";
        case T_OPERATOR: return "Operator";
        case T_TYPE: return "Type";
        case T_POINTER_TYPE: return "Pointer Type";
        case T_DOUBLE_COLON: return "Double Colon";
        case T_L_PAREN: return "Left Paren.";
        case T_R_PAREN: return "Right Paren.";
        case T_L_BRACE: return "Left Brace";
        case T_R_BRACE: return "Right Brace";
        case T_L_BRACKET: return "Left Bracket";
        case T_R_BRACKET: return "Right Bracket";
        case T_STRING: return "String";
        case T_SEMICOLON: return "Semicolon";
        case T_COMMENT: return "Comment";
        case T_COMMA: return "Comma";
        case T_L_ANGLE_BRACKET: return "Left Angle Br.";
        case T_R_ANGLE_BRACKET: return "Right Angle Br.";
        case T_MACRO_CALL: return "Macro Call";
        case T_DOT: return "Dot";
        case T_EQUAL_SIGN: return "Equal Sign";
        case T_NOT_EQUAL_SIGN: return "Not Equal Sign";
        case T_EXCLAMATION_MARK: return "Exclamation Mark";
        case T_COLON: return "Colon";
        case T_HASH_SIGN: return "Hash Sign";
        case T_IMPORT: return "Import";
        case T_ANNOTATION: return "Annotation";
        case T_UNKNOWN: return "Unknown";
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

void free_tokens(Token** tokens, size_t token_count) {
    for (size_t i = 0; i < token_count; i++) {
        free_token(tokens[i]);
    }
    free(tokens);
}

void add_token(Token*** tokens, size_t* token_count, TokenType type, const char* value, size_t line, size_t column, const char* filename) {
    Token* token = make_token(type, value, line, column, filename);
    *tokens = (Token**)realloc(*tokens, (*token_count + 1) * sizeof(Token*));
    (*tokens)[*token_count] = token;
    (*token_count)++;
}

void print_token_table_header() {
    printf("\n|-%.*s-|-%.*s-|-%.*s-|-%.*s-|-%.*s-|\n",
           TYPE_WIDTH, "---------------",
           VALUE_WIDTH, "--------------------",
           LINE_WIDTH, "------",
           COL_WIDTH, "------",
           FILENAME_WIDTH, "--------------------");

    printf("| %-*s | %-*s | %-*s | %-*s | %-*s |\n",
           TYPE_WIDTH, "TYPE",
           VALUE_WIDTH, "VALUE",
           LINE_WIDTH, "LINE",
           COL_WIDTH, "COL",
           FILENAME_WIDTH, "FILENAME");

    printf("|-%.*s-|-%.*s-|-%.*s-|-%.*s-|-%.*s-|\n",
           TYPE_WIDTH, "---------------",
           VALUE_WIDTH, "--------------------",
           LINE_WIDTH, "------",
           COL_WIDTH, "------",
           FILENAME_WIDTH, "--------------------");
}

void print_token(Token* token) {
    printf("| %-*s | %-*s | %-*zu | %-*zu | %-*s |\n",
           TYPE_WIDTH, token_type_to_str(token->type),
           VALUE_WIDTH, token->value,
           LINE_WIDTH, token->line,
           COL_WIDTH, token->column,
           FILENAME_WIDTH, token->filename);
}

void print_token_table_footer() {
    printf("|-%.*s-|-%.*s-|-%.*s-|-%.*s-|-%.*s-|\n",
           TYPE_WIDTH, "---------------",
           VALUE_WIDTH, "--------------------",
           LINE_WIDTH, "------",
           COL_WIDTH, "------",
           FILENAME_WIDTH, "--------------------");
}

void print_tokens(Token** tokens, size_t token_count, TokenType* token_type) {
    print_token_table_header();

    for (size_t i = 0; i < token_count; i++) {
        if (token_type != NULL && tokens[i]->type != *token_type) {
            continue;
        }

        // if (tokens[i]->type == T_COMMENT) {
        //     continue;
        // }

        print_token(tokens[i]);
    }

    print_token_table_footer();
}

void tokenize_line(const char* line, size_t line_number, const char* filename, Token*** buffer, size_t* token_count) {
    if (line == NULL || filename == NULL) {
        printf("Line or filename is NULL\n");
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

        // Handle double colon
        if (line[i] == ':' && i + 1 < len && line[i + 1] == ':') {
            add_token(buffer, token_count, T_DOUBLE_COLON, "::", line_number, column, filename);
            i++;  // Skip the second colon
            continue;
        } else if (line[i] == '"') {
            size_t start = i;
            i++;
            while (i < len && line[i] != '"') {
                i++;
            }

            if (i >= len) {
                break;
            }

            char* string = strndup(line + start + 1, i - start - 1);
            if (string == NULL) {
                continue;
            }

            add_token(buffer, token_count, T_STRING, string, line_number, column, filename);
            free(string);
        } else if (isalpha(line[i])) {
            size_t start = i;
            while (i < len && (isalnum(line[i]) || line[i] == '_')) {
                i++;
            }
            char* identifier = strndup(line + start, i - start);
            if (identifier == NULL) {
                continue;
            }

            // Check if this is a type and followed by a pointer
            int is_type = (
                strcmp(identifier, "u8") == 0 ||
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
            );

            // Look ahead for pointer symbol
            size_t next_pos = i;
            while (next_pos < len && isspace(line[next_pos])) {
                next_pos++;
            }

            if (is_type && next_pos < len && line[next_pos] == '*') {
                char* pointer_type = (char*)malloc(strlen(identifier) + 2);
                sprintf(pointer_type, "%s*", identifier);
                add_token(buffer, token_count, T_POINTER_TYPE, pointer_type, line_number, column, filename);
                free(pointer_type);
                i = next_pos;
            } else if (strcmp(identifier, "pub") == 0 ||
                      strcmp(identifier, "fn") == 0 ||
                      strcmp(identifier, "return") == 0 ||
                      strcmp(identifier, "if") == 0 ||
                      strcmp(identifier, "else") == 0 ||
                      strcmp(identifier, "elif") == 0 ||
                      strcmp(identifier, "defer") == 0 ||
                      strcmp(identifier, "test") == 0) {
                add_token(buffer, token_count, T_KEYWORD, identifier, line_number, column, filename);
            } else if (is_type) {
                add_token(buffer, token_count, T_TYPE, identifier, line_number, column, filename);
            } else {
                // If it starts with import read until ; and add as a single token
                if (strcmp(identifier, "import") == 0) {
                    size_t start = i;
                    while (i < len && line[i] != ';') {
                        i++;
                    }

                    char* import = strndup(line + start + 1, i - start - 1); // Skip the space and the ';'
                    if (import == NULL) {
                        continue;
                    }
                    i++;

                    add_token(buffer, token_count, T_IMPORT, import, line_number, column, filename);
                    free(import);
                } else {
                    // Check if the next character is a exclamation mark
                    // if so this is a macro call
                    if (next_pos < len && line[next_pos] == '!') {
                        char* macro_call = (char*)malloc(strlen(identifier) + 2);
                        sprintf(macro_call, "%s!", identifier);
                        add_token(buffer, token_count, T_MACRO_CALL, macro_call, line_number, column, filename);
                        free(macro_call);
                        i++;
                    } else {
                        add_token(buffer, token_count, T_IDENTIFIER, identifier, line_number, column, filename);
                    }
                }
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
        } else if (line[i] == '=') {
            if (i + 1 < len && line[i + 1] == '=') {
                add_token(buffer, token_count, T_EQUAL_SIGN, "==", line_number, column, filename);
                i++;
            } else {
                add_token(buffer, token_count, T_OPERATOR, "=", line_number, column, filename);
            }
        } else if (line[i] == '!') {
            if (i + 1 < len && line[i + 1] == '=') {
                add_token(buffer, token_count, T_NOT_EQUAL_SIGN, "!=", line_number, column, filename);
                i++;
            } else {
                add_token(buffer, token_count, T_EXCLAMATION_MARK, "!", line_number, column, filename);
            }
        }

        else if (strchr("+-*/", line[i])) {
            if (line[i] == '/' && i + 1 < len && line[i + 1] == '/') {
                size_t start = i;
                while (i < len && line[i] != '\n') {
                    i++;
                }

                // Add comments too
                // char* comment = strndup(line + start, i - start);
                // if (comment == NULL) {
                //     i--;
                //     continue;
                // }

                // add_token(buffer, token_count, T_COMMENT, comment, line_number, column, filename);
                // free(comment);

                i--;
            } else {
                char operator[2] = {line[i], '\0'};
                add_token(buffer, token_count, T_OPERATOR, operator, line_number, column, filename);
            }
        } else if (line[i] == '(') {
            add_token(buffer, token_count, T_L_PAREN, "(", line_number, column, filename);
        } else if (line[i] == ')') {
            add_token(buffer, token_count, T_R_PAREN, ")", line_number, column, filename);
        } else if (line[i] == '{') {
            add_token(buffer, token_count, T_L_BRACE, "{", line_number, column, filename);
        } else if (line[i] == '}') {
            add_token(buffer, token_count, T_R_BRACE, "}", line_number, column, filename);
        } else if (line[i] == '[') {
            add_token(buffer, token_count, T_L_BRACKET, "[", line_number, column, filename);
        } else if (line[i] == ']') {
            add_token(buffer, token_count, T_R_BRACKET, "]", line_number, column, filename);
        } else if (line[i] == ';') {
            add_token(buffer, token_count, T_SEMICOLON, ";", line_number, column, filename);
        } else if (line[i] == ',') {
            add_token(buffer, token_count, T_COMMA, ",", line_number, column, filename);
        } else if (line[i] == '<') {
            add_token(buffer, token_count, T_L_ANGLE_BRACKET, "<", line_number, column, filename);
        } else if (line[i] == '>') {
            add_token(buffer, token_count, T_R_ANGLE_BRACKET, ">", line_number, column, filename);
        } else if (line[i] == '.') {
            add_token(buffer, token_count, T_DOT, ".", line_number, column, filename);
        } else if (line[i] == ':') {
            add_token(buffer, token_count, T_COLON, ":", line_number, column, filename);
        } else if (line[i] == '#') {
            // If the next token is a [ this is an annotation, in that case we need to
            // read until the next ] and add as a single token
            // Otherwise this is a hash sign

            size_t next_pos = i + 1;
            while (next_pos < len && isspace(line[next_pos])) {
                next_pos++;
            }

            if (next_pos < len && line[next_pos] == '[') {
                size_t start = i;
                while (i < len && line[i] != ']') {
                    i++;
                }

                char* annotation = strndup(line + start + 2, i - start - 2);
                if (annotation == NULL) {
                    continue;
                }
                i++;

                add_token(buffer, token_count, T_ANNOTATION, annotation, line_number, column, filename);
                free(annotation);
            } else {
                add_token(buffer, token_count, T_HASH_SIGN, "#", line_number, column, filename);
            }
        } else {
            char unknown[2] = {line[i], '\0'};
            add_token(buffer, token_count, T_UNKNOWN, unknown, line_number, column, filename);
        }
    }
}
