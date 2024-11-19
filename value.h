#ifndef VALUE_H
#define VALUE_H

#include "parser.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  VALUE_INT,
  VALUE_STRING,
  VALUE_FUNCTION,
  VALUE_NULL,
  VALUE_BOOL,
  VALUE_RETURN // should not be used by the user, only internally used for
               // returning from blocks
} ValueType;

typedef struct Value Value;
struct Value {
  ValueType type;
  union {
    int64_t intVal;
    char *string;
    FunctionExpression funciton;
    struct Value *returnVal;
    bool boolean;
  } value;
};

Value newStringValue(char *string);
Value newIntValue(int64_t intval);
Value newFunctionValue(FunctionExpression function);
// will allocate the provided value on the heap and wrap it in a return value
Value newReturnValue(Value returnVal);
Value newBoolValue(bool boolean);
Value newNullValue();

char *valueToString(Value value);
bool valueToBool(Value value);

#endif // !VALUE_H
