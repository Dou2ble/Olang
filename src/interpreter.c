#include "interpreter.h"
#include "environment.h"
#include "parser.h"
#include "utils.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration of evalExperssion
Value evalExpression(Environment *env, Expression expression);

Value evalAssignment(Environment *env, AssignmentExpression expression) {
  environmentSet(env, expression.id,
                 evalExpression(env, *expression.expression));
  return newNullValue();
}

Value evalBlock(Environment *env, BlockExpression expression) {
  Environment blockEnv = newEnvironment(env);

  for (size_t e = 0; e < expression.expressionCount; e++) {
    Value result = evalExpression(&blockEnv, expression.expressions[e]);

    if (result.type == VALUE_RETURN) {
      return result;
    }
  }

  return newNullValue();
}

Value evalCall(Environment *env, CallExpression expression) {
  // check that the function being called actually exsists
  Value function = environmentGet(env, expression.callee);

  if (function.type != VALUE_FUNCTION) {
    return newNullValue();
  }
  if (function.value.funciton.parameterCount != expression.argumentsCount) {
    return newNullValue();
  }

  // create a new environment for the function
  Environment newEnv = newEnvironment(env);
  for (size_t a = 0; a < expression.argumentsCount; a++) {
    environmentSet(&newEnv, function.value.funciton.parameters[a],
                   evalExpression(env, expression.arguments[a]));
  }

  // call the functions expression with the new environment
  Value result = evalExpression(&newEnv, *function.value.funciton.body);
  // return the value if it was a return and unwrap it
  if (result.type == VALUE_RETURN) {
    return *result.value.returnVal;
  } else {
    return result;
  }
}

Value evalVariableDeclaration(Environment *env,
                              VariableDeclarationExpression expression) {
  environmentSet(env, expression.id,
                 evalExpression(env, *expression.expression));

  return newNullValue();
}

Value evalBinary(Environment *env, BinaryExpression expression) {
  Value left = evalExpression(env, *expression.left);
  Value right = evalExpression(env, *expression.right);

  switch (expression.op) {
    // multiplicative
  case BINOP_DIVISION:
    // division requires two integers
    if (left.type != VALUE_INT || right.type != VALUE_INT) {
      return newNullValue();
    }
    return newIntValue(left.value.intVal / right.value.intVal);
  case BINOP_MULTIPLICATION:
    // multiplication between two integers
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newIntValue(left.value.intVal * right.value.intVal);
    } // multplication between string and int (repetition)
    else if ((left.type == VALUE_INT && right.type == VALUE_STRING) ||
             (left.type == VALUE_STRING && right.type == VALUE_INT)) {
      char *string;
      int64_t repetitions;
      if (left.type == VALUE_STRING) {
        string = left.value.string;
        repetitions = right.value.intVal;
      } else {
        repetitions = left.value.intVal;
        string = right.value.string;
      }

      char *repeatedString = repeatString(string, repetitions);
      return newStringValue(repeatedString);
    }
    return newNullValue();
    // additive
  case BINOP_MODULUS:
    // modulo requires two integers
    if (left.type != VALUE_INT || right.type != VALUE_INT) {
      return newNullValue();
    }
    return newIntValue(left.value.intVal % right.value.intVal);
  case BINOP_MINUS:
    // minus requires two integers
    if (left.type != VALUE_INT || right.type != VALUE_INT) {
      return newNullValue();
    }
    // TODO: add support for removing substring from string
    return newIntValue(left.value.intVal - right.value.intVal);
  case BINOP_PLUS:
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newIntValue(left.value.intVal + right.value.intVal);
    } else if (left.type == VALUE_STRING && right.type == VALUE_STRING) {
      char *result =
          malloc(strlen(left.value.string) + strlen(right.value.string) + 1);
      strcat(result, left.value.string);
      strcat(result, right.value.string);

      newStringValue(result);
    }
    break;
    // comparative
  case BINOP_IS_EQUAL:
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newBoolValue(left.value.intVal == right.value.intVal);
    } else if (left.type == VALUE_STRING && right.type == VALUE_STRING) {
      return newBoolValue(strcmp(left.value.string, right.value.string) == 0);
    }
    break;
  case BINOP_IS_NOT_EQUAL:
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newBoolValue(left.value.intVal != right.value.intVal);
    } else if (left.type == VALUE_STRING && right.type == VALUE_STRING) {
      return newBoolValue(strcmp(left.value.string, right.value.string) != 0);
    }
    break;
  case BINOP_IS_GREATER_THAN:
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newBoolValue(left.value.intVal > right.value.intVal);
    }
    break;
  case BINOP_IS_GREATER_THAN_OR_EQUAL:
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newBoolValue(left.value.intVal >= right.value.intVal);
    }
    break;
  case BINOP_IS_LESS_THAN:
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newBoolValue(left.value.intVal < right.value.intVal);
    }
    break;
  case BINOP_IS_LESS_THAN_OR_EQUAL:
    if (left.type == VALUE_INT && right.type == VALUE_INT) {
      return newBoolValue(left.value.intVal <= right.value.intVal);
    }
    break;
  }

  return newNullValue();
}

Value evalConditional(Environment *env, ConditionalExpression expression) {
  switch (expression.type) {
  case CONDITIONAL_IF:
    if (valueToBool(evalExpression(env, *expression.test))) {
      return evalExpression(env, *expression.body);
    }
    break;
  case CONDITIONAL_WHILE:
    while (true) {
      if (!valueToBool(evalExpression(env, *expression.test))) {
        break;
      }
      Value result = evalExpression(env, *expression.body);
      if (result.type == VALUE_RETURN) {
        return result;
      }
    }
    break;
  case CONDITIONAL_FOR:;
    Environment forEnv = newEnvironment(env);
    evalExpression(&forEnv, *expression.init);
    while (true) {
      if (!valueToBool(evalExpression(&forEnv, *expression.test))) {
        break;
      }
      Value result = evalExpression(&forEnv, *expression.body);
      if (result.type == VALUE_RETURN) {
        return result;
      }
      evalExpression(&forEnv, *expression.update);
    }
    break;
  }

  return newNullValue();
}

Value evalExpression(Environment *env, Expression expression) {
  switch (expression.type) {
  case EXPRESSION_INT:
    return newIntValue(expression.value.intVal);
  case EXPRESSION_STRING:
    return newStringValue(expression.value.string);
  case EXPRESSION_FUNCTION:
    return newFunctionValue(expression.value.function);
  case EXPRESSION_BOOL:
    return newBoolValue(expression.value.boolean);
  case EXPRESSION_RETURN:
    return newReturnValue(evalExpression(env, *expression.value.returnVal));
  case EXPRESSION_BLOCK:
    return evalBlock(env, expression.value.block);
  case EXPRESSION_VARIABLE_DECLARATION:
    return evalVariableDeclaration(env, expression.value.variableDeclaration);
  case EXPRESSION_IDENTIFIER:
    return environmentGet(env, expression.value.identifier);
  case EXPRESSION_CALL:
    return evalCall(env, expression.value.call);
  case EXPRESSION_BINARY:
    return evalBinary(env, expression.value.binary);
  case EXPRESSION_CONDITIONAL:
    return evalConditional(env, expression.value.conditional);
  case EXPRESSION_ASSIGNMENT:
    return evalAssignment(env, expression.value.assignment);
  };

  return newNullValue();
};

int eval(char *source) {
  Program ast = parse(source);

  Environment env = newEnvironment(NULL);
  Value result;
  for (size_t e = 0; e < ast.expressionCount; e++) {
    result = evalExpression(&env, ast.expressions[e]);

    // check if the result from the expression is a return
    if (result.type == VALUE_RETURN) {
      if (result.value.returnVal->type != VALUE_INT) {
        perror("Tried to return from top level without int type");
        exit(1);
      }
      // most operating systems don't support return codes lower than zore or
      // higher than 255
      if (result.value.returnVal->value.intVal < 0 ||
          result.value.returnVal->value.intVal > 255) {
        exit(255);
      }

      return ((int)result.value.returnVal->value.intVal);
    }
  };

  return 0;
}

// TODO page 145 aka 143
