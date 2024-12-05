#include "parser.h"
#include "lexer.h"
#include "slog.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char *source;
  size_t tokenCount;
  Token *tokens;
  size_t t;
} Parser;

void printExpression(const Expression expression);

void expectToken(Parser *parser, TokenType type) {
  if (parser->tokens[parser->t].type != type) {
    // slog("Expected int token in int expression");
    slogRegion(parser->source, parser->tokens[parser->t].region, ERROR,
               "Unexpected token found during parsing");
    exit(1);
  }
}

// Forward declaration of parseExpression
Expression parseExpression(Parser *parser);

int64_t parseInt(Parser *parser) {
  int64_t expression;
  expectToken(parser, TOKEN_INT);
  expression = parser->tokens[parser->t].value.intVal;
  parser->t++;

  return expression;
}

char *parseString(Parser *parser) {
  char *expression;
  expectToken(parser, TOKEN_STRING);
  expression = parser->tokens[parser->t].value.string;
  parser->t++;

  return expression;
}

Expression parsePrimary(Parser *parser) {
  Expression expression;
  switch (parser->tokens[parser->t].type) {
  case TOKEN_INT:
    expression.type = EXPRESSION_INT;
    expression.value.intVal = parseInt(parser);
    break;
  case TOKEN_STRING:
    expression.type = EXPRESSION_STRING;
    expression.value.string = parseString(parser);
    break;
  case TOKEN_BOOL:
    expression.type = EXPRESSION_BOOL;
    expression.value.boolean = parser->tokens[parser->t].value.boolean;
    parser->t++;
    break;
  case TOKEN_OPEN_PAREN:
    parser->t++;
    expression = parseExpression(parser);
    expectToken(parser, TOKEN_CLOSE_PAREN);
    parser->t++;
    break;
  default:
    perror("unexpected token while parsing expression");
    slogRegion(parser->source, parser->tokens[parser->t].region, ERROR,
               "Unexpected token found while parsing expression");
    exit(1);
  }

  return expression;
}

Expression parseMultiplicative(Parser *parser) {
  Expression left = parsePrimary(parser);

  while (parser->tokens[parser->t].type == TOKEN_DIVISION ||
         parser->tokens[parser->t].type == TOKEN_MULTIPLICATION ||
         parser->tokens[parser->t].type == TOKEN_MODULUS) {
    BinaryOperator op;
    switch (parser->tokens[parser->t].type) {
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
    parser->t++;
    Expression right = parsePrimary(parser);

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

Expression parseAdditive(Parser *parser) {
  Expression left = parseMultiplicative(parser);

  while (parser->tokens[parser->t].type == TOKEN_PLUS ||
         parser->tokens[parser->t].type == TOKEN_MINUS) {
    BinaryOperator op;
    switch (parser->tokens[parser->t].type) {
    case TOKEN_PLUS:
      op = BINOP_PLUS;
      break;
    case TOKEN_MINUS:
      op = BINOP_MINUS;
      break;
    default:
      abort();
    }
    parser->t++;
    Expression right = parseMultiplicative(parser);

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

Expression parseComparitive(Parser *parser) {
  Expression left = parseAdditive(parser);

  while (parser->tokens[parser->t].type == TOKEN_IS_EQUAL ||
         parser->tokens[parser->t].type == TOKEN_IS_NOT_EQUAL ||
         parser->tokens[parser->t].type == TOKEN_IS_GREATER_THAN ||
         parser->tokens[parser->t].type == TOKEN_IS_GREATER_THAN_OR_EQUAL ||
         parser->tokens[parser->t].type == TOKEN_IS_LESS_THAN ||
         parser->tokens[parser->t].type == TOKEN_IS_LESS_THAN_OR_EQUAL) {
    BinaryOperator op;
    switch (parser->tokens[parser->t].type) {
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
    parser->t++;
    Expression right = parseAdditive(parser);

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

BlockExpression parseBlock(Parser *parser) {
  BlockExpression expression;
  expression.expressionCount = 0;
  expression.expressions = NULL;

  expectToken(parser, TOKEN_OPEN_BRACE);
  parser->t++;

  while (parser->tokens[parser->t].type != TOKEN_CLOSE_BRACE) {
    // make the statements array longer
    expression.expressionCount++;
    expression.expressions =
        realloc(expression.expressions,
                sizeof(Expression) * expression.expressionCount);
    expression.expressions[expression.expressionCount - 1] =
        parseExpression(parser);
    printf("length: %zu\n", expression.expressionCount);
  }

  // skip the closing brace
  parser->t++;

  return expression;
}

IfExpression parseIf(Parser *parser) {
  IfExpression expression;

  expectToken(parser, TOKEN_KEYWORD_IF);
  parser->t++;

  expression.test = malloc(sizeof(Expression));
  *expression.test = parseExpression(parser);

  expression.body = parseBlock(parser);

  return expression;
}

WhileLoopExpression parseWhileLoop(Parser *parser) {
  WhileLoopExpression expression;

  expectToken(parser, TOKEN_KEYWORD_WHILE);
  parser->t++;

  expression.test = malloc(sizeof(Expression));
  *expression.test = parseExpression(parser);

  expression.body = parseBlock(parser);

  return expression;
}

ForLoopExpression parseForLoop(Parser *parser) {
  ForLoopExpression expression;

  expectToken(parser, TOKEN_KEYWORD_FOR);
  parser->t++;

  expression.init = malloc(sizeof(Expression));
  *expression.init = parseExpression(parser);
  expression.test = malloc(sizeof(Expression));
  *expression.test = parseExpression(parser);
  expression.update = malloc(sizeof(Expression));
  *expression.update = parseExpression(parser);

  expression.body = parseBlock(parser);

  return expression;
}

char *parseIdentifier(Parser *parser) {
  char *expression;

  expectToken(parser, TOKEN_IDENTIFIER);
  expression = parser->tokens[parser->t].value.identifier;
  parser->t++;

  return expression;
}

CallExpression parseCall(Parser *parser) {
  CallExpression expression;

  expectToken(parser, TOKEN_IDENTIFIER);
  expression.callee = parser->tokens[parser->t].value.identifier;
  parser->t++;

  expectToken(parser, TOKEN_OPEN_PAREN);
  parser->t++;

  expression.arguments = NULL;
  expression.argumentsCount = 0;

  while (parser->tokens[parser->t].type != TOKEN_CLOSE_PAREN) {
    expression.argumentsCount++;

    expression.arguments = realloc(
        expression.arguments, sizeof(Expression) * expression.argumentsCount);
    expression.arguments[expression.argumentsCount - 1] =
        parseExpression(parser);
  }

  // skip the close paren
  parser->t++;

  return expression;
}

FunctionExpression parseFunction(Parser *parser) {
  FunctionExpression expression;

  expectToken(parser, TOKEN_KEYWORD_FUN);
  parser->t++;

  expectToken(parser, TOKEN_OPEN_PAREN);
  parser->t++;

  expression.parameters = NULL;
  expression.parameterCount = 0;
  while (parser->tokens[parser->t].type != TOKEN_CLOSE_PAREN) {
    expectToken(parser, TOKEN_IDENTIFIER);
    expression.parameterCount++;

    expression.parameters = realloc(expression.parameters,
                                    sizeof(char *) * expression.parameterCount);
    expression.parameters[expression.parameterCount - 1] =
        parser->tokens[parser->t].value.identifier;

    parser->t++;
  }

  // skip the close paren
  parser->t++;

  expression.body = parseBlock(parser);
  return expression;
}

Expression *parseReturn(Parser *parser) {
  Expression *returnVal = malloc(sizeof(Expression));

  expectToken(parser, TOKEN_KEYWORD_RETUN);
  parser->t++;

  *returnVal = parseExpression(parser);
  return returnVal;
}

VariableDeclarationExpression parseVaraibleDeclaration(Parser *parser) {
  VariableDeclarationExpression expression;

  switch (parser->tokens[parser->t].type) {
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
  parser->t++;

  expectToken(parser, TOKEN_IDENTIFIER);
  expression.id = parser->tokens[parser->t].value.identifier;
  parser->t++;

  expectToken(parser, TOKEN_EQUAL_SIGN);
  parser->t++;

  expression.expression = malloc(sizeof(Expression));
  *expression.expression = parseExpression(parser);

  return expression;
}

AssignmentExpression parseAssignment(Parser *parser) {
  AssignmentExpression expression;

  expectToken(parser, TOKEN_IDENTIFIER);
  expression.id = parser->tokens[parser->t].value.identifier;
  parser->t++;

  expectToken(parser, TOKEN_EQUAL_SIGN);
  parser->t++;

  expression.expression = malloc(sizeof(Expression));
  *expression.expression = parseExpression(parser);

  return expression;
}

Expression parseExpression(Parser *parser) {
  Expression expression;
  expression.region.start = parser->tokens[parser->t].region.start;

  switch (parser->tokens[parser->t].type) {
  case TOKEN_OPEN_BRACE:
    expression.type = EXPRESSION_BLOCK;
    expression.value.block = parseBlock(parser);
    break;
  case TOKEN_KEYWORD_FUN:
    expression.type = EXPRESSION_FUNCTION;
    expression.value.function = parseFunction(parser);
    break;
  case TOKEN_IDENTIFIER:
    // if the token after the identifier is an ( it is a function call,
    // example: print()
    if (parser->t < parser->tokenCount - 2) {
      if (parser->tokens[(parser->t) + 1].type == TOKEN_OPEN_PAREN) {
        expression.type = EXPRESSION_CALL;
        expression.value.call = parseCall(parser);
      } else if (parser->tokens[(parser->t) + 1].type == TOKEN_EQUAL_SIGN) {
        expression.type = EXPRESSION_ASSIGNMENT;
        expression.value.assignment = parseAssignment(parser);
      } else {
        expression.type = EXPRESSION_IDENTIFIER;
        expression.value.identifier = parseIdentifier(parser);
      }
    } else {
      expression.type = EXPRESSION_IDENTIFIER;
      expression.value.identifier = parseIdentifier(parser);
    }
    break;
  case TOKEN_KEYWORD_RETUN:
    expression.type = EXPRESSION_RETURN;
    expression.value.returnVal = parseReturn(parser);
    break;
  case TOKEN_KEYWORD_IF:
    expression.type = EXPRESSION_IF;
    expression.value.ifExpression = parseIf(parser);
    break;
  case TOKEN_KEYWORD_WHILE:
    expression.type = EXPRESSION_WHILE_LOOP;
    expression.value.whileLoop = parseWhileLoop(parser);
    break;
  case TOKEN_KEYWORD_FOR:
    expression.type = EXPRESSION_FOR_LOOP;
    expression.value.forLoop = parseForLoop(parser);
    break;
  case TOKEN_KEYWORD_LET:
  case TOKEN_KEYWORD_MUT:
    expression.type = EXPRESSION_VARIABLE_DECLARATION;
    expression.value.variableDeclaration = parseVaraibleDeclaration(parser);
    break;
  default:
    expression = parseComparitive(parser);
  }

  expression.region.end = parser->tokens[parser->t].region.end;
  return expression;
}

Program parse(char *source) {
  Program ast;
  ast.expressionCount = 0;
  ast.expressions = NULL;

  Parser parser;
  parser.source = source;
  parser.tokens = tokenize(&parser.tokenCount, source);
  parser.t = 0;

  // Parse the ast
  while (parser.t < parser.tokenCount) {
    // make the statements array longer
    ast.expressionCount++;
    ast.expressions =
        realloc(ast.expressions, sizeof(Expression) * ast.expressionCount);
    ast.expressions[ast.expressionCount - 1] = parseExpression(&parser);
  }

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

    Expression functionBody;
    functionBody.type = EXPRESSION_BLOCK;
    functionBody.value.block = expression.value.function.body;
    printExpression(functionBody);

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
    case BINOP_IS_LESS_THAN:
      printf("<");
      break;
    case BINOP_IS_LESS_THAN_OR_EQUAL:
      printf("<=");
      break;
    case BINOP_IS_GREATER_THAN:
      printf(">");
      break;
    case BINOP_IS_GREATER_THAN_OR_EQUAL:
      printf(">=");
      break;
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
  case EXPRESSION_IF:
    printf("if(test=");
    printExpression(*expression.value.ifExpression.test);
    printf(" ");

    Expression ifBody;
    ifBody.type = EXPRESSION_BLOCK;
    ifBody.value.block = expression.value.ifExpression.body;
    printExpression(ifBody);

    printf(")");
    break;
  case EXPRESSION_WHILE_LOOP:
    printf("while(test=");
    printExpression(*expression.value.whileLoop.test);
    printf(" ");

    Expression whileBody;
    whileBody.type = EXPRESSION_BLOCK;
    whileBody.value.block = expression.value.whileLoop.body;
    printExpression(whileBody);

    printf(")");
    break;
  case EXPRESSION_FOR_LOOP:
    printf("for(init=");
    printExpression(*expression.value.forLoop.init);
    printf(" ");
    printf(" test=");
    printExpression(*expression.value.forLoop.test);
    printf(" update=");
    printExpression(*expression.value.forLoop.update);

    printf(" ");
    Expression forBody;
    forBody.type = EXPRESSION_BLOCK;
    forBody.value.block = expression.value.forLoop.body;
    printExpression(forBody);

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
