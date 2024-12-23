#ifndef AST_H
#define AST_H

#include <stddef.h>

// Enum to represent the type of an AST node
typedef enum {
    AST_LITERAL,      // Literal values (numbers, strings, etc.)
    AST_VARIABLE,     // Variable reference
    AST_BINARY_OP,    // Binary operation (e.g., +, -, *)
    AST_UNARY_OP,     // Unary operation (e.g., -, !)
    AST_FUNCTION_CALL,// Function call
    AST_FUNCTION_DEF, // Function definition
    AST_BLOCK,        // Code block
    AST_IF,           // If-else statement
    AST_WHILE,        // While loop
    AST_RETURN,       // Return statement
    AST_ASSIGNMENT,   // Variable assignment
    AST_TYPE_DECL,    // Type declaration
    AST_STRUCT_DEF,   // Struct definition
    AST_STRUCT_ACCESS,// Struct member access
    AST_CAST,         // Type casting
} ASTNodeType;

// Enum to represent binary operators
typedef enum {
    BIN_ADD,   // +
    BIN_SUB,   // -
    BIN_MUL,   // *
    BIN_DIV,   // /
    BIN_MOD,   // %
    BIN_AND,   // &&
    BIN_OR,    // ||
    BIN_EQ,    // ==
    BIN_NEQ,   // !=
    BIN_LT,    // <
    BIN_GT,    // >
    BIN_LE,    // <=
    BIN_GE     // >=
} BinaryOperator;

// Enum to represent unary operators
typedef enum {
    UNARY_NEGATE,  // -
    UNARY_NOT      // !
} UnaryOperator;

// Forward declaration of ASTNode for recursive references
typedef struct ASTNode ASTNode;

// Struct to represent an AST node
struct ASTNode {
    ASTNodeType type;     // Type of the AST node
    union {
        // Literal value (AST_LITERAL)
        struct {
            char* value; // Literal value as a string
        } literal;

        // Variable reference (AST_VARIABLE)
        struct {
            char* name;  // Variable name
        } variable;

        // Binary operation (AST_BINARY_OP)
        struct {
            BinaryOperator op;  // Operator
            ASTNode* left;      // Left operand
            ASTNode* right;     // Right operand
        } binary_op;

        // Unary operation (AST_UNARY_OP)
        struct {
            UnaryOperator op;   // Operator
            ASTNode* operand;   // Operand
        } unary_op;

        // Function call (AST_FUNCTION_CALL)
        struct {
            char* name;         // Function name
            ASTNode** args;     // Arguments
            size_t arg_count;   // Number of arguments
        } function_call;

        // Function definition (AST_FUNCTION_DEF)
        struct {
            char* name;         // Function name
            char** param_names; // Parameter names
            char** param_types; // Parameter types
            size_t param_count; // Number of parameters
            char** return_type; // Return type
            ASTNode* body;      // Function body
        } function_def;

        // Code block (AST_BLOCK)
        struct {
            ASTNode** statements; // List of statements
            size_t statement_count; // Number of statements
        } block;

        // If-else statement (AST_IF)
        struct {
            ASTNode* condition;   // Condition
            ASTNode* then_branch; // Then branch
            ASTNode* else_branch; // Else branch (optional)
        } if_statement;

        // While loop (AST_WHILE)
        struct {
            ASTNode* condition; // Condition
            ASTNode* body;      // Loop body
        } while_loop;

        // Return statement (AST_RETURN)
        struct {
            ASTNode* value; // Return value (optional)
        } return_statement;

        // Variable assignment (AST_ASSIGNMENT)
        struct {
            char* name;       // Variable name
            ASTNode* value;   // Assigned value
        } assignment;

        // Type declaration (AST_TYPE_DECL)
        struct {
            char* name;       // Variable name
            char* type;       // Type name
        } type_decl;

        // Struct definition (AST_STRUCT_DEF)
        struct {
            char* name;         // Struct name
            char** field_names; // Field names
            char** field_types; // Field types
            size_t field_count; // Number of fields
        } struct_def;

        // Struct member access (AST_STRUCT_ACCESS)
        struct {
            ASTNode* struct_expr; // Struct expression
            char* member_name;   // Member name
        } struct_access;

        // Type casting (AST_CAST)
        struct {
            ASTNode* expr;      // Expression to cast
            char* target_type;  // Target type
        } cast;
    };
};

// Function declarations
ASTNode* create_literal_node(const char* value);
ASTNode* create_variable_node(const char* name);
ASTNode* create_binary_op_node(BinaryOperator op, ASTNode* left, ASTNode* right);
ASTNode* create_unary_op_node(UnaryOperator op, ASTNode* operand);
ASTNode* create_function_call_node(const char* name, ASTNode** args, size_t arg_count);
ASTNode* create_function_def_node(const char* name, char** param_names, char** param_types, size_t param_count, char** return_type, ASTNode* body);
ASTNode* create_block_node(ASTNode** statements, size_t statement_count);
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);
ASTNode* create_return_node(ASTNode* value);
ASTNode* create_assignment_node(const char* name, ASTNode* value);
ASTNode* create_type_decl_node(const char* name, const char* type);
ASTNode* create_struct_def_node(const char* name, char** field_names, char** field_types, size_t field_count);
ASTNode* create_struct_access_node(ASTNode* struct_node, const char* field_name);
ASTNode* create_cast_node(const char* type, ASTNode* value);
void free_ast_node(ASTNode* node);
void print_ast_node(ASTNode* node, size_t indent);

#endif // AST_H
