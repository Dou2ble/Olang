// The Olang value system
#ifndef VALUE_H
#define VALUE_H

#include "parser.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Value Value;

typedef enum FunctionType {
  FUNCTION_BUILTIN, // builtin functions that run c code
  FUNCTION_DEFINED, // user defined functions declared using olang code
} FunctionType;

typedef struct FunctionValue {
  FunctionType type;
  size_t parameterCount;
  union {
    Value (*builtin)(Value *arguments);
    struct {
      char **parameters;
      BlockExpression body;
    } defined;
  } function;
} FunctionValue;

typedef enum ValueType {
  VALUE_STRING,
  VALUE_INT,
  VALUE_BOOL,
  VALUE_FUNCTION,
  VALUE_NULL,

  // system values that should not be accessed by the user directly
  VALUE_SYSTEM_RETURN,
  VALUE_SYSTEM_EXCEPTION
} ValueType;

Value newStringValue(char *value);
Value newIntValue(int64_t value);
Value newBoolValue(bool value);
Value newDefinedFunctionValue(FunctionExpression value);
Value newNullValue();
Value newSystemReturn(Value value);
Value newSystemException(char *value);

// free any memory that the value might be using on the heap
void destroyValue(Value value);

struct Value {
  ValueType type;
  union {
    char *string;
    int64_t integer;
    bool boolean;
    FunctionValue function;
    Value *systemReturn;
    char *systemException;
  } value;
};

#endif
