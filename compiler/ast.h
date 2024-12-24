#ifndef AST_H
#define AST_H

#include <stddef.h>

// Enum to represent the type of an AST node
typedef enum {
    AST_VARIABLE_DEF, // Variable definition
    AST_VARIABLE_ASSIGNMENT, // Variable assignment
    AST_LITERAL,      // Constant literal values (numbers, strings, etc.)
    AST_REFERENCE,    // Variable values (number, string, etc.)
    AST_BINARY_OP,    // Binary operation (e.g., +, -, *)
    AST_UNARY_OP,     // Unary operation (e.g., -, !)
    AST_FUNCTION_CALL,// Function call
    AST_FUNCTION_DEF, // Function definition
    AST_BLOCK,        // Code block
    AST_IF,           // If-else statement
    AST_WHILE,        // While loop
    AST_RETURN,       // Return statement
    AST_DEFER,        // Defer statement
    AST_ASSIGNMENT,   // Variable assignment
    AST_TYPE_DECL,    // Type declaration
    AST_STRUCT_DEF,   // Struct definition
    AST_STRUCT_ACCESS,// Struct member access
    AST_CAST,         // Type casting
    AST_ARRAY_DEF,    // Array literal
    AST_ARRAY_ACCESS, // Array indexing (e.g., arr[3])
    AST_ARRAY_ASSIGNMENT, // Array element assignment (e.g., arr[3] = 10)
    AST_LITERAL_ARRAY // Array of literals

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
        // Variable definition (AST_VARIABLE_DEF)
        struct {
            char* name;       // Variable name
            char* type;       // Variable type
            ASTNode* initializer;   // Initial value (optional)
        } variable_def;

        // Variable assignment (AST_VARIABLE_ASSIGNMENT)
        struct {
            char* name;       // Variable name
            ASTNode* value;   // Assigned value
        } variable_assignment;

        // Literal value (AST_LITERAL)
        struct {
            char* value; // Literal value as a string
        } literal;

        // Variable reference (AST_VARIABLE)
        struct {
            char* name;  // Name of the reference
            ASTNode* child; // Children of the reference (say std.iostream, then iostream is a child of std)
        } reference;

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
            int is_public;      // Public or private
            char** param_names; // Parameter names
            char** param_types; // Parameter types
            size_t param_count; // Number of parameters
            char* return_type; // Return type
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

        // Defer statement (AST_DEFER)
        struct {
            ASTNode* value; // Defer value
        } defer_statement;

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

        // Array definition (AST_ARRAY_DEF)
        struct {
            char* name;       // Variable name
            char* type;       // Variable type
            ASTNode* initializer;   // Initial value (optional)
        } array_def;

        // Array access (AST_ARRAY_ACCESS)
        struct {
            char* reference;   // Array variable name
            ASTNode* index;    // Index expression
            ASTNode* child;   // Nested array access (optional) (e.g., arr#1#2)
        } array_access;

        // Array assignment (AST_ARRAY_ASSIGNMENT)
        struct {
            char* reference;   // Array variable name
            ASTNode* index;    // Index expression
            ASTNode* value;    // Assigned value
        } array_assignment;

        // Array of literals (AST_LITERAL_ARRAY)
        struct {
            ASTNode** values;   // Array of literal values
            size_t value_count; // Number of literal values
        } literal_array;
    };
};

// Function declarations
ASTNode* create_variable_def_node(const char* name, const char* type, ASTNode* initializer);
ASTNode* create_variable_assignment_node(const char* name, ASTNode* value);
ASTNode* create_literal_node(const char* value);
ASTNode* create_reference_node(const char* name);
ASTNode* create_binary_op_node(BinaryOperator op, ASTNode* left, ASTNode* right);
ASTNode* create_unary_op_node(UnaryOperator op, ASTNode* operand);
ASTNode* create_function_call_node(const char* name, ASTNode** args, size_t arg_count);
ASTNode* create_function_def_node(const char* name, int is_public, char** param_names, char** param_types, size_t param_count, char* return_type, ASTNode* body);
ASTNode* create_block_node(ASTNode** statements, size_t statement_count);
ASTNode* create_if_node(ASTNode* condition, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);
ASTNode* create_return_node(ASTNode* value);
ASTNode* create_defer_node(ASTNode* value);
ASTNode* create_assignment_node(const char* name, ASTNode* value);
ASTNode* create_type_decl_node(const char* name, const char* type);
ASTNode* create_struct_def_node(const char* name, char** field_names, char** field_types, size_t field_count);
ASTNode* create_struct_access_node(ASTNode* struct_node, const char* field_name);
ASTNode* create_cast_node(const char* type, ASTNode* value);
ASTNode* create_array_def_node(const char* name, const char* type, ASTNode* initializers);
ASTNode* create_array_access_node(const char* reference, ASTNode* index);
ASTNode* create_array_assignment_node(const char* reference, ASTNode* index, ASTNode* value);
ASTNode* create_literal_array_node(ASTNode** values, size_t value_count);
void free_ast_node(ASTNode* node);
void print_ast_node(ASTNode* node, size_t indent);
BinaryOperator str_to_binary_op(const char* str);

#endif // AST_H
