#include "lexer.h"
#include "utils.h"
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
    free_ast_node(parser->ast_root);
    free(parser);
}

Token* current_token(Parser* parser) {
    if (parser->current >= parser->token_count) return NULL;
    return parser->tokens[parser->current];
}

void error(Parser* parser, const char* message) {
    Token* token = current_token(parser);
    if (token) {
        fprintf(stderr, "\033[31mError: %s:%zu:%zu\n\t %s.\n\033[0m",
                token->filename, token->line, token->column,
                message);
    } else {
        fprintf(stderr, "\033[31mError: %s at end of input.\n\033[0m", message);
    }
    exit(1);
}

Token* next_token(Parser* parser) {
    if (parser->current + 1 >= parser->token_count) {
        error(parser, "Unexpected end of input");
    };
    parser->current++;
    return parser->tokens[parser->current];
}

Token* peak_token(Parser* parser) {
    if (parser->current + 1 >= parser->token_count) {
        error(parser, "Unexpected end of input");
    };
    return parser->tokens[parser->current + 1];
}


ASTNode* parse_function(Parser* parser, int is_public) {
    // The next token should be an identifier, namely the name of the function
    Token* name = next_token(parser);
    if (name->type != T_IDENTIFIER) {
        error(parser, "Expected identifier after fn keyword");
    }

    // Set one for the param names and one for the types but set to NULL because we may have none
    char** param_names = NULL;
    char** param_types = NULL;

    // Then if the next is < we expect params
    Token* next = next_token(parser);
    if (next->type == T_L_ANGLE_BRACKET) {
        // Parse the parameters
        size_t param_count = 0;
        param_names = malloc(sizeof(char*) * 8);
        param_types = malloc(sizeof(char*) * 8);

        // Loop through the parameters un til we get the closing >
        while (1) {
            Token* param_type = next_token(parser);
            if (param_type->type != T_TYPE) {
                // Add the used value in the message
                char* message = malloc(strlen("Expected valid type as return type, got ") + strlen(param_type->value) + 1);
                if (message == NULL) {
                    error(parser, "Out of memory");
                }
                sprintf(message, "Expected valid type as return type, got %s", param_type->value);
                error(parser, message);
            }

            Token* param_name = next_token(parser);
            if (param_name->type != T_IDENTIFIER) {
                error(parser, "Expected identifier as parameter name");
            }

            param_names[param_count] = strdup_c(param_name->value);
            param_types[param_count] = strdup_c(param_type->value);
            param_count++;

            Token* comma_or_close = next_token(parser);
            if (comma_or_close->type == T_COMMA) {
                continue;
            } else if (comma_or_close->type == T_R_ANGLE_BRACKET) {
                break;
            } else {
                error(parser, "Expected ',' or '>' to add more parameters, or close the parameter list");
            }
        }

        // Now we expect the double colon
        Token* double_colon = next_token(parser);
        if (double_colon->type != T_DOUBLE_COLON) {
            error(parser, "Expected '::' after function parameters");
        }

        // Now we expect the return type
        Token* return_type = next_token(parser);
        if (return_type->type != T_TYPE) {
            // Add the used value in the message
            char* message = malloc(strlen("Expected valid type as return type, got ") + strlen(return_type->value) + 1);
            if (message == NULL) {
                error(parser, "Out of memory");
            }
            sprintf(message, "Expected valid type as return type, got %s", return_type->value);
            error(parser, message);
        }

        // Now we expect the opening brace
        Token* open_brace = next_token(parser);
        if (open_brace->type != T_L_BRACE) {
            error(parser, "Expected '{' after function declaration");
        }

        ASTNode** body_statements = NULL;
        size_t body_stmt_count = 0;

        while (1) {
            Token* token = current_token(parser);
            if (token->type == T_R_BRACE) {
                break;
            }

            if (token->type == T_KEYWORD) {
                // Could be if, elif, else, return, defer etc.

            } else if (token->type == T_TYPE) {
                // Then the next item has to be an identifier, namely the name of the variable
                Token* variable_name = next_token(parser);
                if (variable_name->type != T_IDENTIFIER) {
                    error(parser, "Expected identifier after type declaration");
                }

                ASTNode* initializer = NULL;

                // Lets peak at the next token, if that is an equal sign we have a direct assignment
                Token* equal_or_semicolon = next_token(parser);
                if (equal_or_semicolon->type == T_EQUAL_SIGN) {
                    // The next token is the type
                    Token* value = next_token(parser);
                    // Now this can be a literal, a reference, a function call, or whatever some other expression
                    if (value->type == T_NUMBER) {
                        // TODO: This could be a mathematical expression
                        initializer = create_literal_node(value->value);
                    } else if (value->type == T_STRING) {
                        // TODO: String concatenation hello?
                        initializer = create_literal_node(value->value);
                    } else if (value->type == T_IDENTIFIER) {
                        // TODO: This could be a reference to another variable, a function call, or a struct name
                    } else {
                        error(parser, "Expected literal or reference after '='");
                    }
                } else if (equal_or_semicolon->type != T_SEMICOLON) {
                    error(parser, "Expected '=' or ';' after type declaration");
                }

                ASTNode* variable_def = create_variable_def_node(variable_name->value, token->value, initializer);
                // Add to body statements
                body_statements = realloc(body_statements, sizeof(ASTNode*) * (body_stmt_count + 1));
                body_statements[body_stmt_count++] = variable_def;
            } else if (token->type == T_POINTER_TYPE) {
                // TODO: Has to be variable def
            } else if (token->type == T_L_BRACE) {
                // TODO: Has to be array variable def
            } else if (token->type == T_IDENTIFIER) {
                // TODO: Could refer to a function call (in or some namespace)
            } else if (token->type == T_ANNOTATION) {
                // TODO: Annotate something that is relevant for the next token
            }

            // Move to the next token
            parser->current++;
        }

        ASTNode* function_block = create_block_node(body_statements, body_stmt_count);

        // Now we should have all the info we need to create the function node
        return create_function_def_node(name->value, param_names, param_types, param_count, return_type->value, function_block);
    }

    return NULL;
}

ASTNode* parse_statement(Parser* parser) {
    Token* token = parser->tokens[parser->current];

    // If we are importing something
    if (token->type == T_KEYWORD) {
        // If this is pub then we expect a fn keyword next
        if (strcmp(token->value, "pub") == 0) {
            // Get the next token
            Token* next = next_token(parser);
            if (next->type != T_KEYWORD ||
                strcmp(next->value, "fn") != 0
            ) {
                error(parser, "Expected fn keyword after pub");
            }

            return parse_function(parser, 1);
        }
        else if (strcmp(token->value, "fn") == 0) {
            return parse_function(parser, 0);
        }
    }
    else {
        // error(parser, "Expected keyword or import statement");
        parser->current++;
    }

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

        parser->current++;
    }

    if (count > 0) {
        parser->ast_root = create_block_node(statements, count);
    }
}
