#include "value.h"

#include <stdlib.h>
#include <string.h>

// will realloc anything that the value might have allocated on the heap
Value cloneValue(Value value) {
  switch (value.type) {
  case VALUE_STRING:;
    size_t stringSize = strlen(value.value.string) + 1;
    char *newString = malloc(stringSize);
    memcpy(newString, value.value.string, stringSize);
    value.value.string = newString;
    break;
  case VALUE_SYSTEM_EXCEPTION:;
    size_t exceptionSize = strlen(value.value.string) + 1;
    char *newException = malloc(exceptionSize);
    memcpy(newException, value.value.string, exceptionSize);
    value.value.string = newException;
    break;
  case VALUE_SYSTEM_RETURN:
    value.value.systemReturn = malloc(sizeof(Value));
    *value.value.systemReturn = cloneValue(*value.value.systemReturn);
    break;
  default:;
  }

  return value;
}

// will free anything that the value might have allocated on the heap
void destroyValue(Value value) {
  switch (value.type) {
  case VALUE_STRING:
    free(value.value.string);
    break;
  case VALUE_SYSTEM_EXCEPTION:
    free(value.value.systemException);
    break;
  case VALUE_SYSTEM_RETURN:
    free(value.value.systemReturn);
    break;
  default:;
  }
}

Value newStringValue(char *value) {
  Value result;
  result.type = VALUE_STRING;

  result.value.string = malloc(strlen(value) + 1);
  strcpy(result.value.string, value);

  return result;
}

Value newIntValue(int64_t value) {
  Value result;
  result.type = VALUE_INT;
  result.value.integer = value;

  return result;
}

Value newBoolValue(bool value) {
  Value result;
  result.type = VALUE_BOOL;
  result.value.boolean = value;

  return result;
}

Value newDefinedFunctionValue(FunctionExpression value) {
  Value result;
  result.type = VALUE_FUNCTION;
  result.value.function.type = FUNCTION_DEFINED;
  result.value.function.parameterCount = value.parameterCount;
  result.value.function.function.defined.parameters = value.parameters;
  result.value.function.function.defined.body = value.body;

  return result;
}

Value newNullValue() {
  Value result;
  result.type = VALUE_NULL;
  return result;
}

Value newSystemReturn(Value value) {
  Value result;
  result.type = VALUE_SYSTEM_RETURN;
  result.value.systemReturn = malloc(sizeof(Value));
  *result.value.systemReturn = cloneValue(value);

  return result;
}

Value newSystemException(char *value) {
  Value result;
  result.type = VALUE_SYSTEM_EXCEPTION;

  result.value.systemException = malloc(strlen(value) + 1);
  strcpy(result.value.systemException, value);

  return result;
};
