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

// This function parses a reference
// Note that the function must be called PRIOR to
// moving the parser's cursor to the identifier that marks the beginning
// of a reference sequence.
// A reference serves as anything that represents a declaration of a variable in response to
// a implicit or explicit type declaration. (Thus variable assignments, return statements, param assignments, defer statements, etc.)
ASTNode* parse_reference(Parser* parser, int is_tracking_function_args) {
    ASTNode* buffer = NULL;
    char* last_ref_name = NULL;

    while (1) {
        Token* next = next_token(parser);

        if (is_tracking_function_args == 1) {
            if (next->type == T_COMMA || next->type == T_R_PAREN) {
                return buffer;
            }
        }

        // If the next token is a ; we know we are done processing
        if (next->type == T_L_PAREN) {
            ASTNode** args = NULL;
            size_t arg_count = 0;

            while (1) {
                Token* next = peak_token(parser);
                if (peak_token(parser)->type == T_COMMA) {
                    parser->current++;
                    continue;
                } else {
                    // Parse the argument
                    ASTNode* arg = parse_reference(parser, 1);
                    args = realloc(args, sizeof(ASTNode*) * (arg_count + 1));
                    if (args == NULL) {
                        error(parser, "Out of memory");
                    }

                    args[arg_count] = arg;
                    arg_count++;

                    // Check current token
                    if (current_token(parser)->type == T_R_PAREN) {
                        // We don't skip past the current token ) because the next iteration of
                        // the loop in will get the next_token
                        break;
                    }
                }
            }

            if (buffer == NULL) {
                buffer = create_function_call_node(last_ref_name, args, arg_count);
            } else {
                ASTNode* current = buffer; // Create a temporary buffer
                while (current->reference.child != NULL) {
                    current = current->reference.child;

                    // If the current child is NULL we need to add the new child to the current child
                    if (current == NULL) {
                        current->reference.child = create_function_call_node(last_ref_name, args, arg_count);
                        break;
                    }
                }
            }
        } else if (next->type == T_DOT) {
            continue;
        } else if (next->type == T_IDENTIFIER) {
            if (buffer == NULL) {
                buffer = create_reference_node(next->value);
                last_ref_name = next->value;
            } else {
                // We know for a matter of fact that this is a reference to some earlier reference
                ASTNode* current = buffer; // Create a temporary buffer
                while (current->reference.child != NULL) {
                    current = current->reference.child;

                    // If the current child is NULL we need to add the new child to the current child
                    if (current == NULL) {
                        current->reference.child = create_reference_node(next->value);
                        last_ref_name = next->value;
                        break;
                    }
                }
            }
        } else if (next->type == T_HASH_SIGN) {
            // This is an array reference
            Token* index = next_token(parser);
            if (index->type != T_NUMBER) {
                error(parser, "Expected number after '#' in array reference");
            }

            ASTNode* index_value_node = create_literal_node(index->value);

            if (buffer == NULL) {
                // Since the buffer is still NULL this must be the first reference
                // thus ref_name is the name of the array
                buffer = create_array_access_node(last_ref_name, index_value_node);
            } else {
                // In this case the array is the child of some other reference
                ASTNode* current = buffer; // Create a temporary buffer
                while (current->reference.child != NULL) {
                    current = current->reference.child;
                    if (current == NULL) {
                        // We must deal with double array access (some_array#1#0)
                        // However, AST_ARRAY_ACCESS has a child field so we
                        // can add the anonymous array access to that as a child
                        // In other cases where this for instance would be a struct we just add
                        // this to a actual reference node
                        buffer->reference.child = create_array_access_node(last_ref_name, index_value_node);
                        break;
                    }
                }
            }
        }
        else if (next->type == T_OPERATOR) {
            // Handle binary operation
            char* operator = next->value;

            // Parse the right-hand side reference
            ASTNode* right = parse_reference(parser, 0);
            if (right == NULL) {
                error(parser, "Expected a valid right-hand side after operator");
            }

            // Create a binary operation node
            ASTNode* binary_op = create_binary_op_node(str_to_binary_op(operator), buffer, right);
            // We return here, partly because the parsing of the right hand sided already caused
            // the cursor to move to the ;

            // Compensate for the cursor
            parser->current--;
            buffer = binary_op;

            // return binary_op;
        }
        else if (next->type == T_NUMBER || next->type == T_STRING) {
            // This is a literal
            ASTNode* literal = create_literal_node(next->value);
            if (buffer == NULL) {
                buffer = literal;
            } else {
                ASTNode* current = buffer; // Create a temporary buffer
                while (current->reference.child != NULL) {
                    current = current->reference.child;

                    // If the current child is NULL we need to add the new child to the current child
                    if (current == NULL) {
                        current->reference.child = literal;
                        break;
                    }
                }
            }
        }
        else if (next->type == T_L_BRACKET) {
            // This is an array initializer [1, 2, 3], NOT a type definition
            ASTNode** array_values = NULL;
            size_t array_value_count = 0;

            while (1) {
                Token* array_value = next_token(parser);
                if (array_value->type == T_NUMBER || array_value->type == T_STRING) {
                    array_values = realloc(array_values, sizeof(ASTNode*) * (array_value_count + 1));
                    if (array_values == NULL) {
                        error(parser, "Out of memory");
                    }

                    array_values[array_value_count] = create_literal_node(array_value->value);
                    array_value_count++;
                } else if (array_value->type == T_COMMA) {
                    continue;
                } else if (array_value->type == T_R_BRACKET) {
                    break;
                } else {
                    printf("Token type: %s\n", parser->tokens[parser->current]->value);
                    error(parser, "Expected number or ']' in array initializer");
                }
            }

            ASTNode* literal_array = create_literal_array_node(array_values, array_value_count);
            if (buffer == NULL) {
                buffer = literal_array;
            } else {
                ASTNode* current = buffer; // Create a temporary buffer
                while (current->reference.child != NULL) {
                    current = current->reference.child;

                    // If the current child is NULL we need to add the new child to the current child
                    if (current == NULL) {
                        current->reference.child = literal_array;
                        break;
                    }
                }
            }
        }
        else if (next->type == T_SEMICOLON) {
            if (buffer == NULL) {
                error(parser, "A reference or function call is missing");
            }
            return buffer;
        } else {
            char* message = malloc(strlen("Unexpected token in reference, got ") + strlen(next->value) + 1);
            if (message == NULL) {
                error(parser, "Out of memory");
            }
            sprintf(message, "Unexpected token in reference, got %s", next->value);
            error(parser, message);
        }
    }

    return NULL;
}

void parse_ast_body(Parser* parser, ASTNode*** body_statements, size_t* body_stmt_count) {
    // Move past '{'
    parser->current++;

    while (1) {
        Token* token = current_token(parser);
        if (token->type == T_R_BRACE) {
            break;
        }

        if (token->type == T_KEYWORD) {
            if (strcmp(token->value, "if") == 0) {
                // Has to be followed by a brace
                Token* open_brace = next_token(parser);
                if (open_brace->type != T_L_PAREN) {
                    error(parser, "Expected '(' after if keyword");
                }

                // TODO: Read the condition
                // for now just loop until yu hit a closing brace
                // DEBUG: This is a temporary solution
                while (1) {
                    Token* next = next_token(parser);
                    if (next->type == T_R_PAREN) {
                        break;
                    }
                }

                Token* open_body_brace = next_token(parser);
                if (open_body_brace->type != T_L_BRACE) {
                    error(parser, "Expected '{' after elif condition");
                }

                ASTNode** if_body_statements = NULL;
                size_t if_body_stmt_count = 0;

                parse_ast_body(parser, &if_body_statements, &if_body_stmt_count);
            } else if (strcmp(token->value, "elif") == 0) {
                // Has to be followed by a brace
                Token* open_brace = next_token(parser);
                if (open_brace->type != T_L_PAREN) {
                    error(parser, "Expected '(' after elif keyword");
                }

                // TODO: Read the condition
                // for now just loop until yu hit a closing brace
                // DEBUG: This is a temporary solution
                while (1) {
                    Token* next = next_token(parser);
                    if (next->type == T_R_PAREN) {
                        break;
                    }
                }

                Token* open_body_brace = next_token(parser);
                if (open_body_brace->type != T_L_BRACE) {
                    error(parser, "Expected '{' after elif condition");
                }

                ASTNode** elif_body_statements = NULL;
                size_t elif_body_stmt_count = 0;

                parse_ast_body(parser, &elif_body_statements, &elif_body_stmt_count);
            } else if (strcmp(token->value, "else") == 0) {
                // Has to be followed by a brace
                Token* open_brace = next_token(parser);
                if (open_brace->type != T_L_BRACE) {
                    error(parser, "Expected '{' after else keyword");
                }

                Token* open_body_brace = next_token(parser);
                if (open_body_brace->type != T_L_BRACE) {
                    error(parser, "Expected '{' after elif condition");
                }

                ASTNode** else_body_statements = NULL;
                size_t else_body_stmt_count = 0;

                parse_ast_body(parser, &else_body_statements, &else_body_stmt_count);
            } else if (strcmp(token->value, "return") == 0) {
                ASTNode* ref = parse_reference(parser, 0);
                ASTNode* return_node = create_return_node(ref);

                *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                if (*body_statements == NULL) {
                    error(parser, "Out of memory");
                }

                (*body_statements)[*body_stmt_count] = return_node;
                (*body_stmt_count)++;
            } else if (strcmp(token->value, "defer") == 0) {
                printf("Defer\n");
                ASTNode* ref = parse_reference(parser, 0);
                ASTNode* return_node = create_defer_node(ref);

                *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                if (*body_statements == NULL) {
                    error(parser, "Out of memory");
                }

                (*body_statements)[*body_stmt_count] = return_node;
                (*body_stmt_count)++;
            } else {
                error(parser, "Unexpected keyword in function body");
            }
        } else if (token->type == T_TYPE || token->type == T_POINTER_TYPE) {
            // If this is the type then the next is the name
            Token* next = next_token(parser);
            if (next->type != T_IDENTIFIER) {
                error(parser, "Expected identifier after type declaration");
            }

            // Then the next has to be either a semicolon or an equal sign
            Token* equal_or_semicolon = next_token(parser);
            if (equal_or_semicolon->type == T_OPERATOR && strcmp(equal_or_semicolon->value, "=") == 0) {
                ASTNode* ref = parse_reference(parser, 0);
                if (ref == NULL) {
                    error(parser, "Expected reference after type declaration");
                }
                ASTNode* variable_def = create_variable_def_node(next->value, token->value, ref);

                *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                if (*body_statements == NULL) {
                    error(parser, "Out of memory");
                }

                // Then we add the variable def to the body
                (*body_statements)[*body_stmt_count] = variable_def;
                (*body_stmt_count)++;
            } else if (equal_or_semicolon->type == T_SEMICOLON) {
                // This is just a type declaration
                ASTNode* type_decl = create_type_decl_node(next->value, token->value);

                *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                if (*body_statements == NULL) {
                    error(parser, "Out of memory");
                }

                // Then we add the variable def to the body
                (*body_statements)[*body_stmt_count] = type_decl;
                (*body_stmt_count)++;
            } else {
                error(parser, "Expected ';' or '=' after type declaration");
            }

            // At this point we should have handled the entire type declaration
            if (current_token(parser)->type != T_SEMICOLON) {
                error(parser, "Expected ';' after type declaration");
            }

        } else if (token->type == T_L_BRACKET) {
            // If this is the type then the next is the name
            Token* arr_type = next_token(parser);
            if (arr_type->type != T_TYPE && arr_type->type != T_POINTER_TYPE) {
                error(parser, "Expected identifier after type declaration");
            }

            Token* close_bracket = next_token(parser);
            if (close_bracket->type != T_R_BRACKET) {
                error(parser, "Expected ']' after array type declaration");
            }

            Token* next = next_token(parser);
            if (next->type != T_IDENTIFIER) {
                error(parser, "Expected identifier after type declaration");
            }

            // Then the next has to be either a semicolon or an equal sign
            Token* equal_or_semicolon = next_token(parser);
            if (equal_or_semicolon->type == T_OPERATOR && strcmp(equal_or_semicolon->value, "=") == 0) {
                // The next one would be some reference
                ASTNode* ref = parse_reference(parser, 0);
                ASTNode* variable_def = create_array_def_node(next->value, token->value, ref);

                *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                if (*body_statements == NULL) {
                    error(parser, "Out of memory");
                }

                // Then we add the variable def to the body
                (*body_statements)[*body_stmt_count] = variable_def;
                (*body_stmt_count)++;
            } else if (equal_or_semicolon->type == T_SEMICOLON) {
                // This is just a type declaration
                ASTNode* type_decl = create_type_decl_node(next->value, token->value);

                *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                if (*body_statements == NULL) {
                    error(parser, "Out of memory");
                }

                // Then we add the variable def to the body
                (*body_statements)[*body_stmt_count] = type_decl;
                (*body_stmt_count)++;
            }
        } else if (token->type == T_IDENTIFIER) {
            // If it is an identifier followed by an identifier we know that hte first one is a type
            // otherwise it would be an assignment
            Token* identifier_or_assign = next_token(parser);
            if (identifier_or_assign->type == T_IDENTIFIER) {
                // Then the next has to be either a semicolon or an equal sign
                Token* equal_or_semicolon = next_token(parser);
                if (equal_or_semicolon->type == T_OPERATOR && strcmp(equal_or_semicolon->value, "=") == 0) {
                    // The next one would be some reference
                    ASTNode* ref = parse_reference(parser, 0);
                    ASTNode* variable_def = create_variable_def_node(token->value, token->value, ref);

                    *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                    if (*body_statements == NULL) {
                        error(parser, "Out of memory");
                    }

                    // Then we add the variable def to the body
                    (*body_statements)[*body_stmt_count] = variable_def;
                    (*body_stmt_count)++;
                } else if (equal_or_semicolon->type == T_SEMICOLON) {
                    // This is just a type declaration
                    ASTNode* type_decl = create_type_decl_node(token->value, token->value);

                    *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                    if (*body_statements == NULL) {
                        error(parser, "Out of memory");
                    }

                    // Then we add the variable def to the body
                    (*body_statements)[*body_stmt_count] = type_decl;
                    (*body_stmt_count)++;
                }
            } else if (identifier_or_assign->type == T_OPERATOR && strcmp(identifier_or_assign->value, "=") == 0) {
                // This is a variable assignment
                ASTNode* ref = parse_reference(parser, 0);
                ASTNode* variable_assignment = create_variable_assignment_node(token->value, ref);

                *body_statements = realloc(*body_statements, sizeof(ASTNode*) * (*body_stmt_count + 1));
                if (*body_statements == NULL) {
                    error(parser, "Out of memory");
                }

                // Then we add the variable def to the body
                (*body_statements)[*body_stmt_count] = variable_assignment;
                (*body_stmt_count)++;
            }
        } else if (token->type == T_ANNOTATION) {
            // TODO: Annotate something that is relevant for the next token
        }

        // Move to the next token
        parser->current++;
    }
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
    size_t param_count = 0;

    // Then if the next is < we expect params
    Token* next = next_token(parser);
    if (next->type == T_L_ANGLE_BRACKET) {
        param_names = malloc(sizeof(char*) * 8);
        param_types = malloc(sizeof(char*) * 8);

        // Loop through the parameters until we get the closing
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

        next = next_token(parser);
    }

    if (next->type != T_DOUBLE_COLON) {
        char* message = malloc(strlen("Expected '::' after function parameters, got ") + strlen(next->value) + 1);
        if (message == NULL) {
            error(parser, "Out of memory");
        }
        sprintf(message, "Expected '::' after function parameters, got %s", next->value);
        error(parser, message);
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
    parse_ast_body(parser, &body_statements, &body_stmt_count);

    ASTNode* function_block = create_block_node(body_statements, body_stmt_count);
    return create_function_def_node(name->value, is_public, param_names, param_types, param_count, return_type->value, function_block);

    return NULL;
}

ASTNode* parse_statement(Parser* parser) {
    Token* token = current_token(parser);

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
        else if (strcmp(token->value, "struct") == 0) {
            // TODO: Parse struct definition
            return NULL;
        } else {
            char* message = malloc(strlen("No support for this keyword: ") + strlen(token->value) + 1);
            if (message == NULL) {
                error(parser, "Out of memory");
            }
            sprintf(message, "No support for this keyword: %s", token->value);
            error(parser, message);
        }
    }
    else {
        // error(parser, "Expected keyword or import statement");
        // parser->current++;
        // print the token
        // printf("Token: %s\n", parser->tokens[parser->current]->value);
    }

    // Move the parser past the last token of the sequence
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
