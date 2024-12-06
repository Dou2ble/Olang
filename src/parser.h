#ifndef PARSER_H
#define PARSER_H

#include "slog.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Forward declaration of Expression
typedef struct Expression Expression;

typedef enum {
  EXPRESSION_INT,
  EXPRESSION_STRING,
  EXPRESSION_CALL,
  EXPRESSION_IDENTIFIER,
  EXPRESSION_BLOCK,
  EXPRESSION_FUNCTION,
  EXPRESSION_RETURN,
  EXPRESSION_VARIABLE_DECLARATION,
  EXPRESSION_BINARY,
  EXPRESSION_FOR_LOOP,
  EXPRESSION_WHILE_LOOP,
  EXPRESSION_IF,
  EXPRESSION_BOOL,
  EXPRESSION_ASSIGNMENT,
} ExpressionType;

typedef struct BlockExpression {
  size_t expressionCount;
  Expression *expressions;
} BlockExpression;

typedef struct {
  Expression *init; // used by for loop
  Expression *test;
  Expression *update; // used by for loop
  BlockExpression body;
} ForLoopExpression;

typedef struct {
  Expression *test;
  BlockExpression body;
} WhileLoopExpression;

typedef struct {
  Expression *test;
  BlockExpression body;
} IfExpression;

typedef enum {
  BINOP_PLUS,                     // +
  BINOP_MINUS,                    // -
  BINOP_MULTIPLICATION,           // *
  BINOP_DIVISION,                 // /
  BINOP_MODULUS,                  // %
  BINOP_IS_EQUAL,                 // ==
  BINOP_IS_NOT_EQUAL,             // !=
  BINOP_IS_GREATER_THAN,          // >
  BINOP_IS_LESS_THAN,             // <
  BINOP_IS_GREATER_THAN_OR_EQUAL, // >=
  BINOP_IS_LESS_THAN_OR_EQUAL,    // <=
} BinaryOperator;

typedef struct BinaryExpression {
  BinaryOperator op;
  Expression *left;
  Expression *right;
} BinaryExpression;

typedef struct {
  char **parameters;
  size_t parameterCount;
  BlockExpression body;
} FunctionExpression;

typedef struct {
  char *callee;
  Expression *arguments;
  size_t argumentsCount;
} CallExpression;

typedef struct {
  char *id;
  bool isMutable;
  Expression *expression;
} VariableDeclarationExpression;

typedef struct {
  char *id;
  Expression *expression;
} AssignmentExpression;

struct Expression {
  ExpressionType type;
  Region region;
  union {
    int64_t intVal;
    char *string;
    char *identifier;
    CallExpression call;
    BlockExpression block;
    FunctionExpression function;
    Expression *returnVal;
    VariableDeclarationExpression variableDeclaration;
    BinaryExpression binary;
    ForLoopExpression forLoop;
    WhileLoopExpression whileLoop;
    IfExpression ifExpression;
    AssignmentExpression assignment;
    bool boolean;
  } value;
};

typedef struct {
  size_t expressionCount;
  Expression *expressions;
} Program;

Program parse(char *source);
void printAst(const Program statement);

#endif // !PARSER_H
