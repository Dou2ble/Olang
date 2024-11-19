#include "parser.h"
#include "lexer.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

size_t tokenCount;
Token *tokens;
size_t t;

// Forward declaration of parseExpression
Expression parseExpression();

int64_t parseInt() {
  int64_t expression;
  if (tokens[t].type != TOKEN_INT) {
    perror("Expected int token in int expression");
    exit(1);
  }
  expression = tokens[t].value.intVal;
  t++;

  return expression;
}

char *parseString() {
  char *expression;
  if (tokens[t].type != TOKEN_STRING) {
    perror("Expected int token in int expression");
    exit(1);
  }

  expression = tokens[t].value.string;
  t++;

  return expression;
}

Expression parsePrimary() {
  Expression expression;
  switch (tokens[t].type) {
  case TOKEN_INT:
    expression.type = EXPRESSION_INT;
    expression.value.intVal = parseInt();
    break;
  case TOKEN_STRING:
    expression.type = EXPRESSION_STRING;
    expression.value.string = parseString();
    break;
  case TOKEN_BOOL:
    expression.type = EXPRESSION_BOOL;
    expression.value.boolean = tokens[t].value.boolean;
    t++;
    break;
  case TOKEN_OPEN_PAREN:
    t++;
    expression = parseExpression();
    if (tokens[t].type != TOKEN_CLOSE_PAREN) {
      printf("Expected closing parenthesis");
    }
    t++;
    break;
  default:
    perror("unexpected token while parsing expression");
    printToken(tokens[t]);
    exit(1);
  }

  return expression;
}

Expression parseMultiplicative() {
  Expression left = parsePrimary();

  while (tokens[t].type == TOKEN_DIVISION ||
         tokens[t].type == TOKEN_MULTIPLICATION ||
         tokens[t].type == TOKEN_MODULUS) {
    BinaryOperator op;
    switch (tokens[t].type) {
    case TOKEN_DIVISION:
      op = BINOP_DIVISION;
      break;
    case TOKEN_MULTIPLICATION:
      op = BINOP_MULTIPLICATION;
      break;
    case TOKEN_MODULUS:
      op = BINOP_MODULUS;
      break;
    default:
      abort();
    }
    t++;
    Expression right = parsePrimary();

    Expression newLeft;
    newLeft.type = EXPRESSION_BINARY;
    newLeft.value.binary.left = malloc(sizeof(Expression));
    *newLeft.value.binary.left = left;
    newLeft.value.binary.right = malloc(sizeof(Expression));
    *newLeft.value.binary.right = right;
    newLeft.value.binary.op = op;

    left = newLeft;
  }

  return left;
}

Expression parseAdditive() {
  Expression left = parseMultiplicative();

  while (tokens[t].type == TOKEN_PLUS || tokens[t].type == TOKEN_MINUS) {
    BinaryOperator op;
    switch (tokens[t].type) {
    case TOKEN_PLUS:
      op = BINOP_PLUS;
      break;
    case TOKEN_MINUS:
      op = BINOP_MINUS;
      break;
    default:
      abort();
    }
    t++;
    Expression right = parseMultiplicative();

    Expression newLeft;
    newLeft.type = EXPRESSION_BINARY;
    newLeft.value.binary.left = malloc(sizeof(Expression));
    *newLeft.value.binary.left = left;
    newLeft.value.binary.right = malloc(sizeof(Expression));
    *newLeft.value.binary.right = right;
    newLeft.value.binary.op = op;

    left = newLeft;
  }

  return left;
}

Expression parseComparitive() {
  Expression left = parseAdditive();

  while (tokens[t].type == TOKEN_IS_EQUAL ||
         tokens[t].type == TOKEN_IS_NOT_EQUAL ||
         tokens[t].type == TOKEN_IS_GREATER_THAN ||
         tokens[t].type == TOKEN_IS_GREATER_THAN_OR_EQUAL ||
         tokens[t].type == TOKEN_IS_LESS_THAN ||
         tokens[t].type == TOKEN_IS_LESS_THAN_OR_EQUAL) {
    BinaryOperator op;
    switch (tokens[t].type) {
    case TOKEN_IS_EQUAL:
      op = BINOP_IS_EQUAL;
      break;
    case TOKEN_IS_NOT_EQUAL:
      op = BINOP_IS_NOT_EQUAL;
      break;
    case TOKEN_IS_GREATER_THAN:
      op = BINOP_IS_GREATER_THAN;
      break;
    case TOKEN_IS_GREATER_THAN_OR_EQUAL:
      op = BINOP_IS_GREATER_THAN_OR_EQUAL;
      break;
    case TOKEN_IS_LESS_THAN:
      op = BINOP_IS_LESS_THAN;
      break;
    case TOKEN_IS_LESS_THAN_OR_EQUAL:
      op = BINOP_IS_LESS_THAN_OR_EQUAL;
      break;
    default:
      abort();
    }
    t++;
    Expression right = parseAdditive();

    Expression newLeft;
    newLeft.type = EXPRESSION_BINARY;
    newLeft.value.binary.left = malloc(sizeof(Expression));
    *newLeft.value.binary.left = left;
    newLeft.value.binary.right = malloc(sizeof(Expression));
    *newLeft.value.binary.right = right;
    newLeft.value.binary.op = op;

    left = newLeft;
  }

  return left;
}

ConditionalExpression parseConditional() {
  ConditionalExpression expression;
  switch (tokens[t].type) {
  case TOKEN_KEYWORD_IF:
    expression.type = CONDITIONAL_IF;
    break;
  case TOKEN_KEYWORD_WHILE:
    expression.type = CONDITIONAL_WHILE;
    break;
  case TOKEN_KEYWORD_FOR:
    expression.type = CONDITIONAL_FOR;
    break;
  default:
    perror("Unexpected token at start of conditional");
    exit(1);
  }
  t++;

  if (expression.type == CONDITIONAL_FOR) {
    expression.init = malloc(sizeof(Expression));
    *expression.init = parseExpression();
  } else {
    expression.init = NULL;
  }
  expression.test = malloc(sizeof(Expression));
  *expression.test = parseExpression();
  if (expression.type == CONDITIONAL_FOR) {
    expression.update = malloc(sizeof(Expression));
    *expression.update = parseExpression();
  } else {
    expression.update = NULL;
  }

  expression.body = malloc(sizeof(Expression));
  *expression.body = parseExpression();

  return expression;
}

BlockExpression parseBlock() {
  BlockExpression expression;
  expression.expressionCount = 0;
  expression.expressions = NULL;

  if (tokens[t].type != TOKEN_OPEN_BRACE) {
    perror("Expected { at start of block statement");
    exit(1);
  }
  t++;

  while (tokens[t].type != TOKEN_CLOSE_BRACE) {
    // make the statements array longer
    expression.expressionCount++;
    expression.expressions =
        realloc(expression.expressions,
                sizeof(Expression) * expression.expressionCount);
    expression.expressions[expression.expressionCount - 1] = parseExpression();
  }

  if (tokens[t].type != TOKEN_CLOSE_BRACE) {
    perror("Expected } at start of block statement");
    exit(1);
  }
  t++;

  return expression;
}

char *parseIdentifier() {
  char *expression;

  if (tokens[t].type != TOKEN_IDENTIFIER) {
    perror("Expected identifier token at identifier expression");
    exit(1);
  }
  expression = tokens[t].value.identifier;
  t++;

  return expression;
}

CallExpression parseCall() {
  CallExpression expression;

  if (tokens[t].type != TOKEN_IDENTIFIER) {
    perror("Expected identifier token at call expression");
    exit(1);
  }
  expression.callee = tokens[t].value.identifier;
  t++;

  if (tokens[t].type != TOKEN_OPEN_PAREN) {
    perror("Expected ( token at call expression");
    exit(1);
  }
  t++;

  expression.arguments = NULL;
  expression.argumentsCount = 0;

  while (tokens[t].type != TOKEN_CLOSE_PAREN) {
    expression.argumentsCount++;

    expression.arguments = realloc(
        expression.arguments, sizeof(Expression) * expression.argumentsCount);
    expression.arguments[expression.argumentsCount - 1] = parseExpression();
  }

  // skip the close paren
  t++;

  return expression;
}

FunctionExpression parseFunction() {
  FunctionExpression expression;

  if (tokens[t].type != TOKEN_KEYWORD_FUN) {
    perror("Expected fun keyword at start of function expression");
    exit(1);
  }
  t++;

  if (tokens[t].type != TOKEN_OPEN_PAREN) {
    perror("Expected ( token at start of function expression");
    exit(1);
  }
  t++;

  expression.parameters = NULL;
  expression.parameterCount = 0;
  while (tokens[t].type != TOKEN_CLOSE_PAREN) {
    if (tokens[t].type != TOKEN_IDENTIFIER) {
      perror("Expected identifier token as parameter");
      exit(1);
    }
    expression.parameterCount++;

    expression.parameters = realloc(expression.parameters,
                                    sizeof(char *) * expression.parameterCount);
    expression.parameters[expression.parameterCount - 1] =
        tokens[t].value.identifier;

    t++;
  }

  // skip the close paren
  t++;

  expression.body = malloc(sizeof(Expression));
  *expression.body = parseExpression();
  return expression;
}

Expression *parseReturn() {
  Expression *returnVal = malloc(sizeof(Expression));

  if (tokens[t].type != TOKEN_KEYWORD_RETUN) {
    perror("Expected return keyword in return statement");
    exit(1);
  }
  t++;

  *returnVal = parseExpression();
  return returnVal;
}

VariableDeclarationExpression parseVaraibleDeclaration() {
  VariableDeclarationExpression expression;

  switch (tokens[t].type) {
  case TOKEN_KEYWORD_LET:
    expression.isMutable = false;
    break;
  case TOKEN_KEYWORD_MUT:
    expression.isMutable = true;
    break;
  default:
    perror("Unexpected token at start of variable statement (expected let or "
           "mut keyword token)");
    exit(1);
  }
  t++;

  if (tokens[t].type != TOKEN_IDENTIFIER) {
    perror("Expected identifier at variable declaration");
  }
  expression.id = tokens[t].value.identifier;
  t++;

  if (tokens[t].type != TOKEN_EQUAL_SIGN) {
    perror("Expected equal sign at variable declaration");
  }
  t++;

  expression.expression = malloc(sizeof(Expression));
  *expression.expression = parseExpression();

  return expression;
}

AssignmentExpression parseAssignment() {
  AssignmentExpression expression;

  if (tokens[t].type != TOKEN_IDENTIFIER) {
    perror("Expected identifier at start of variable assignmeent");
  }
  expression.id = tokens[t].value.identifier;
  t++;

  if (tokens[t].type != TOKEN_EQUAL_SIGN) {
    perror("Expected = at variable assignmeent");
  }
  t++;

  expression.expression = malloc(sizeof(Expression));
  *expression.expression = parseExpression();

  return expression;
}

Expression parseExpression() {
  Expression expression;

  switch (tokens[t].type) {
  case TOKEN_OPEN_BRACE:
    expression.type = EXPRESSION_BLOCK;
    expression.value.block = parseBlock();
    break;
  case TOKEN_KEYWORD_FUN:
    expression.type = EXPRESSION_FUNCTION;
    expression.value.function = parseFunction();
    break;
  case TOKEN_IDENTIFIER:
    // if the token after the identifier is an ( it is a function call,
    // example: print()
    if (t < tokenCount - 2) {
      if (tokens[(t) + 1].type == TOKEN_OPEN_PAREN) {
        expression.type = EXPRESSION_CALL;
        expression.value.call = parseCall();
      } else if (tokens[(t) + 1].type == TOKEN_EQUAL_SIGN) {
        expression.type = EXPRESSION_ASSIGNMENT;
        expression.value.assignment = parseAssignment();
      }
    } else {
      expression.type = EXPRESSION_IDENTIFIER;
      expression.value.identifier = parseIdentifier();
    }
    break;
  case TOKEN_KEYWORD_RETUN:
    expression.type = EXPRESSION_RETURN;
    expression.value.returnVal = parseReturn();
    break;
  case TOKEN_KEYWORD_IF:
  case TOKEN_KEYWORD_WHILE:
  case TOKEN_KEYWORD_FOR:
    expression.type = EXPRESSION_CONDITIONAL;
    expression.value.conditional = parseConditional();
    break;
  case TOKEN_KEYWORD_LET:
  case TOKEN_KEYWORD_MUT:
    expression.type = EXPRESSION_VARIABLE_DECLARATION;
    expression.value.variableDeclaration = parseVaraibleDeclaration();
    break;
  default:
    expression = parseComparitive();
  }

  return expression;
}

Program parse(const char *source) {
  Program ast;
  ast.expressionCount = 0;
  ast.expressions = NULL;

  tokens = tokenize(&tokenCount, source);
  t = 0;

  // Parse the ast
  while (t < tokenCount) {
    // make the statements array longer
    ast.expressionCount++;
    ast.expressions =
        realloc(ast.expressions, sizeof(Expression) * ast.expressionCount);
    ast.expressions[ast.expressionCount - 1] = parseExpression();
  }

  // this will not free all of the strings that tokenize() has allocated on
  // the heap but that is intended since these strings are also in the ast and
  // will be needed during evaluation free(tokens);
  tokens = NULL;

  return ast;
};

// Printing functions for debugging
void printExpression(const Expression expression) {
  switch (expression.type) {
  case EXPRESSION_ASSIGNMENT:
    printf("assignment(%s = ", expression.value.assignment.id);
    printExpression(*expression.value.assignment.expression);
    printf(")");
    break;
  case EXPRESSION_INT:
    printf("int(%" PRId64 ")", expression.value.intVal);
    break;
  case EXPRESSION_STRING:
    printf("string(%s)", expression.value.string);
    break;
  case EXPRESSION_FUNCTION:
    printf("function(");
    for (size_t p = 0; p < expression.value.function.parameterCount; p++) {
      printf("%s ", expression.value.function.parameters[p]);
    }
    printExpression(*expression.value.function.body);
    printf(")");
    break;
  case EXPRESSION_RETURN:
    printf("return(");
    printExpression(*expression.value.returnVal);
    printf(")");
    break;
  case EXPRESSION_CALL:
    printf("call(%s", expression.value.call.callee);
    for (size_t a = 0; a < expression.value.call.argumentsCount; a++) {
      printf(" ");
      printExpression(expression.value.call.arguments[a]);
    }
    printf(")");
    break;
  case EXPRESSION_IDENTIFIER:
    printf("identifier(%s)", expression.value.identifier);
    break;
  case EXPRESSION_VARIABLE_DECLARATION:
    printf("variableDeclaration(");
    if (expression.value.variableDeclaration.isMutable) {
      printf("mutable");
    }
    printf("%s = ", expression.value.variableDeclaration.id);
    printExpression(*expression.value.variableDeclaration.expression);
    printf(")");
    break;
  case EXPRESSION_BLOCK:
    printf("block(");
    for (size_t e = 0; e < expression.value.block.expressionCount; e++) {
      printExpression(expression.value.block.expressions[e]);
    }
    printf(")");
    break;
  case EXPRESSION_BINARY:
    printf("binop(");
    printExpression(*expression.value.binary.left);
    printf(" ");
    switch (expression.value.binary.op) {
    case BINOP_DIVISION:
      printf("/");
      break;
    case BINOP_MINUS:
      printf("-");
      break;
    case BINOP_MODULUS:
      printf("%%");
      break;
    case BINOP_PLUS:
      printf("+");
      break;
    case BINOP_MULTIPLICATION:
      printf("*");
      break;
    case BINOP_IS_EQUAL:
      printf("==");
      break;
    case BINOP_IS_NOT_EQUAL:
      printf("!=");
      break;
    }
    printf(" ");
    printExpression(*expression.value.binary.right);
    printf(")");
    break;
  case EXPRESSION_BOOL:
    if (expression.value.boolean) {
      printf("bool(true)");
    } else {
      printf("bool(false)");
    }
    break;
  case EXPRESSION_CONDITIONAL:
    printf("conditional(");
    switch (expression.value.conditional.type) {
    case CONDITIONAL_FOR:
      printf("for");
      break;
    case CONDITIONAL_WHILE:
      printf("while");
      break;
    case CONDITIONAL_IF:
      printf("if");
      break;
    }

    if (expression.value.conditional.type == CONDITIONAL_FOR) {
      printf(" init=");
      printExpression(*expression.value.conditional.init);
    }
    printf(" test=");
    printExpression(*expression.value.conditional.test);
    if (expression.value.conditional.type == CONDITIONAL_FOR) {
      printf(" update=");
      printExpression(*expression.value.conditional.update);
    }

    printf(" body=");
    printExpression(*expression.value.conditional.body);
    printf(")");
    break;
  }
}

void printAst(const Program ast) {
  for (size_t e = 0; e < ast.expressionCount; e++) {
    printExpression(ast.expressions[e]);
    printf("\n");
  }
}
