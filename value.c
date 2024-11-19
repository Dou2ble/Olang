#include "value.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Value newStringValue(char *string) {
  Value value;
  value.type = VALUE_STRING;
  value.value.string = string;

  return value;
}

Value newIntValue(int64_t intVal) {
  Value value;
  value.type = VALUE_INT;
  value.value.intVal = intVal;

  return value;
}

Value newFunctionValue(FunctionExpression function) {
  Value value;
  value.type = VALUE_FUNCTION;
  value.value.funciton = function;

  return value;
}

// will allocate the provided value on the heap and wrap it in a return value
Value newReturnValue(Value returnVal) {
  Value value;
  value.type = VALUE_RETURN;
  value.value.returnVal = malloc(sizeof(value));
  *value.value.returnVal = returnVal;

  return value;
}

Value newNullValue() {
  Value value;
  value.type = VALUE_NULL;
  return value;
}

Value newBoolValue(bool boolean) {
  Value value;
  value.type = VALUE_BOOL;
  value.value.boolean = boolean;

  return value;
}

char *valueToString(Value value) {
  char *string = NULL;

  switch (value.type) {
  case VALUE_STRING:
    string = value.value.string;
    break;
  case VALUE_BOOL:
    if (value.value.boolean) {
      string = malloc(5);
      strcpy(string, "true");
    } else {
      string = malloc(6);
      strcpy(string, "false");
    }
    break;
  case VALUE_FUNCTION:;
    string = malloc(3);
    strcpy(string, "<>");
    break;
  case VALUE_INT:;
    char buffer[21]; // Enough to hold int64_t as a string
    sprintf(buffer, "%" PRId64, value.value.intVal);

    string = malloc(strlen(buffer) + 1);
    strcpy(string, buffer);
    break;
  case VALUE_RETURN:
    return valueToString(*value.value.returnVal);
  case VALUE_NULL:
    string = malloc(5);
    strcpy(string, "null");
    return string;
  }

  return string;
};

bool valueToBool(Value value) {
  return value.type == VALUE_BOOL && value.value.boolean;
}
