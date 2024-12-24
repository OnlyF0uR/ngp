#include "ast.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* binary_op_to_str(BinaryOperator op) {
    switch (op) {
        case BIN_ADD: return "+";
        case BIN_SUB: return "-";
        case BIN_MUL: return "*";
        case BIN_DIV: return "/";
        case BIN_MOD: return "%";
        case BIN_AND: return "&&";
        case BIN_OR: return "||";
        case BIN_EQ: return "==";
        case BIN_NEQ: return "!=";
        case BIN_LT: return "<";
        case BIN_GT: return ">";
        case BIN_LE: return "<=";
        case BIN_GE: return ">=";
        default: return "Invalid";
    }
}

char* unary_op_to_str(UnaryOperator op) {
    switch (op) {
        case UNARY_NEGATE: return "-";
        case UNARY_NOT: return "!";
        default: return "Invalid";
    }
}

ASTNode* create_variable_def_node(const char* name, const char* type, ASTNode* initializer) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_VARIABLE_DEF;
    node->variable_def.name = strdup_c(name);
    node->variable_def.type = strdup_c(type);
    node->variable_def.initializer = initializer;
    return node;
}

ASTNode* create_variable_assignment_node(const char* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_VARIABLE_ASSIGNMENT;
    node->variable_assignment.name = strdup_c(name);
    node->variable_assignment.value = value;
    return node;
}

ASTNode* create_literal_node(const char* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_LITERAL;
    node->literal.value = strdup_c(value);
    return node;
}

ASTNode* create_reference_node(const char* name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_REFERENCE;
    node->reference.name = strdup_c(name);
    return node;
}

ASTNode* create_binary_op_node(BinaryOperator op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->binary_op.op = op;
    node->binary_op.left = left;
    node->binary_op.right = right;
    return node;
}

ASTNode* create_unary_op_node(UnaryOperator op, ASTNode* operand) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_UNARY_OP;
    node->unary_op.op = op;
    node->unary_op.operand = operand;
    return node;
}

ASTNode* create_function_call_node(const char* name, ASTNode** args, size_t arg_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_CALL;
    node->function_call.name = strdup_c(name);
    node->function_call.args = args;
    node->function_call.arg_count = arg_count;
    return node;
}

ASTNode* create_function_def_node(const char* name, char** param_names, char** param_types, size_t param_count, char* return_type, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_DEF;
    node->function_def.name = strdup_c(name);
    node->function_def.param_names = param_names;
    node->function_def.param_types = param_types;
    node->function_def.param_count = param_count;
    node->function_def.return_type = return_type;
    node->function_def.body = body;
    return node;
}

ASTNode* create_block_node(ASTNode** statements, size_t statement_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->block.statements = statements;
    node->block.statement_count = statement_count;
    return node;
}

ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_IF;
    node->if_statement.condition = condition;
    node->if_statement.then_branch = then_branch;
    node->if_statement.else_branch = else_branch;
    return node;
}

ASTNode* create_while_node(ASTNode* condition, ASTNode* body) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->while_loop.condition = condition;
    node->while_loop.body = body;
    return node;
}

ASTNode* create_return_node(ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    node->return_statement.value = value;
    return node;
}

ASTNode* create_assignment_node(const char* name, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->assignment.name = strdup_c(name);
    node->assignment.value = value;
    return node;
}

ASTNode* create_type_decl_node(const char* name, const char* type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_TYPE_DECL;
    node->type_decl.name = strdup_c(name);
    node->type_decl.type = strdup_c(type);
    return node;
}

ASTNode* create_struct_def_node(const char* name, char** field_names, char** field_types, size_t field_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_STRUCT_DEF;
    node->struct_def.name = strdup_c(name);
    node->struct_def.field_names = field_names;
    node->struct_def.field_types = field_types;
    node->struct_def.field_count = field_count;
    return node;
}

ASTNode* create_struct_access_node(ASTNode* struct_node, const char* field_name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_STRUCT_ACCESS;
    node->struct_access.struct_expr = struct_node;
    node->struct_access.member_name = strdup_c(field_name);
    return node;
}

ASTNode* create_cast_node(const char* type, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_CAST;
    node->cast.target_type = strdup_c(type);
    node->cast.expr = value;
    return node;
}

void free_ast_node(ASTNode* node) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_VARIABLE_DEF:
            free(node->variable_def.name);
            free(node->variable_def.type);
            free_ast_node(node->variable_def.initializer);
            break;
        case AST_VARIABLE_ASSIGNMENT:
            free(node->variable_assignment.name);
            free_ast_node(node->variable_assignment.value);
            break;
        case AST_LITERAL:
            free(node->literal.value);
            break;
        case AST_REFERENCE:
            free(node->reference.name);
            break;
        case AST_BINARY_OP:
            free_ast_node(node->binary_op.left);
            free_ast_node(node->binary_op.right);
            break;
        case AST_UNARY_OP:
            free_ast_node(node->unary_op.operand);
            break;
        case AST_FUNCTION_CALL:
            free(node->function_call.name);
            for (size_t i = 0; i < node->function_call.arg_count; i++) {
                free_ast_node(node->function_call.args[i]);
            }
            free(node->function_call.args);
            break;
        case AST_FUNCTION_DEF:
            free(node->function_def.name);
            for (size_t i = 0; i < node->function_def.param_count; i++) {
                free(node->function_def.param_names[i]);
            }
            free(node->function_def.param_names);
            free_ast_node(node->function_def.body);
            break;
        case AST_BLOCK:
            for (size_t i = 0; i < node->block.statement_count; i++) {
                free_ast_node(node->block.statements[i]);
            }
            free(node->block.statements);
            break;
        case AST_IF:
            free_ast_node(node->if_statement.condition);
            free_ast_node(node->if_statement.then_branch);
            free_ast_node(node->if_statement.else_branch);
            break;
        case AST_WHILE:
            free_ast_node(node->while_loop.condition);
            free_ast_node(node->while_loop.body);
            break;
        case AST_RETURN:
            free_ast_node(node->return_statement.value);
            break;
        case AST_ASSIGNMENT:
            free(node->assignment.name);
            free_ast_node(node->assignment.value);
            break;
        case AST_TYPE_DECL:
            free(node->type_decl.name);
            free(node->type_decl.type);
            break;
        case AST_STRUCT_DEF:
            free(node->struct_def.name);
            for (size_t i = 0; i < node->struct_def.field_count; i++) {
                free(node->struct_def.field_names[i]);
                free(node->struct_def.field_types[i]);
            }
            free(node->struct_def.field_names);
            free(node->struct_def.field_types);
            break;
        case AST_STRUCT_ACCESS:
            free_ast_node(node->struct_access.struct_expr);
            free(node->struct_access.member_name);
            break;
        case AST_CAST:
            free(node->cast.target_type);
            free_ast_node(node->cast.expr);
            break;

        default:
            break;
    }
}

void print_ast_node(ASTNode* node, size_t indent) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_VARIABLE_DEF:
            printf("%*sVariable Def: %s %s\n", (int)indent, "", node->variable_def.name, node->variable_def.type);
            print_ast_node(node->variable_def.initializer, indent + 2);
            break;
        case AST_VARIABLE_ASSIGNMENT:
            printf("%*sVariable Assignment: %s\n", (int)indent, "", node->variable_assignment.name);
            print_ast_node(node->variable_assignment.value, indent + 2);
            break;
        case AST_LITERAL:
            printf("%*sLiteral: %s\n", (int)indent, "", node->literal.value);
            break;
        case AST_REFERENCE:
            printf("%*sReference: %s\n", (int)indent, "", node->reference.name);
            break;
        case AST_BINARY_OP:
            printf("%*sBinary Op: %s\n", (int)indent, "", binary_op_to_str(node->binary_op.op));
            print_ast_node(node->binary_op.left, indent + 2);
            print_ast_node(node->binary_op.right, indent + 2);
            break;
        case AST_UNARY_OP:
            printf("%*sUnary Op: %s\n", (int)indent, "", unary_op_to_str(node->unary_op.op));
            print_ast_node(node->unary_op.operand, indent + 2);
            break;
        case AST_FUNCTION_CALL:
            printf("%*sFunction Call: %s\n", (int)indent, "", node->function_call.name);
            for (size_t i = 0; i < node->function_call.arg_count; i++) {
                print_ast_node(node->function_call.args[i], indent + 2);
            }
            break;
        case AST_FUNCTION_DEF:
            printf("%*sFunction Def: %s\n", (int)indent, "", node->function_def.name);
            for (size_t i = 0; i < node->function_def.param_count; i++) {
                printf("%*sParam: %s\n", (int)indent + 2, "", node->function_def.param_names[i]);
            }
            print_ast_node(node->function_def.body, indent + 2);
            break;
        case AST_BLOCK:
            printf("%*sBlock:\n", (int)indent, "");
            for (size_t i = 0; i < node->block.statement_count; i++) {
                print_ast_node(node->block.statements[i], indent + 2);
            }
            break;
        case AST_IF:
            printf("%*sIf:\n", (int)indent, "");
            print_ast_node(node->if_statement.condition, indent + 2);
            print_ast_node(node->if_statement.then_branch, indent + 2);
            print_ast_node(node->if_statement.else_branch, indent + 2);
            break;
        case AST_WHILE:
            printf("%*sWhile:\n", (int)indent, "");
            print_ast_node(node->while_loop.condition, indent + 2);
            print_ast_node(node->while_loop.body, indent + 2);
            break;
        case AST_RETURN:
            printf("%*sReturn:\n", (int)indent, "");
            print_ast_node(node->return_statement.value, indent + 2);
            break;
        case AST_ASSIGNMENT:
            printf("%*sAssignment: %s\n", (int)indent, "", node->assignment.name);
            print_ast_node(node->assignment.value, indent + 2);
            break;
        case AST_TYPE_DECL:
            printf("%*sType Decl: %s %s\n", (int)indent, "", node->type_decl.name, node->type_decl.type);
            break;
        case AST_STRUCT_DEF:
            printf("%*sStruct Def: %s\n", (int)indent, "", node->struct_def.name);
            for (size_t i = 0; i < node->struct_def.field_count; i++) {
                printf("%*sField: %s %s\n", (int)indent + 2, "", node->struct_def.field_names[i], node->struct_def.field_types[i]);
            }
            break;
        case AST_STRUCT_ACCESS:
            printf("%*sStruct Access: %s\n", (int)indent, "", node->struct_access.member_name);
            print_ast_node(node->struct_access.struct_expr, indent + 2);
            break;
        case AST_CAST:
            printf("%*sCast: %s\n", (int)indent, "", node->cast.target_type);
            print_ast_node(node->cast.expr, indent + 2);
            break;
        default:
            break;
    }
}
