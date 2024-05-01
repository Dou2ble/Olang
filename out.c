//go:build ignore

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include <gc.h>

void panic(char *message) {
  printf("==> OttoC Panic: %s", message);
  exit(1);
}

typedef enum {
  OttoCVariableKindString,
  OttoCVariableKindInteger,
  OttoCVariableKindBoolean,
  OttoCVariableKindArray
} OttoCVariableKind;

typedef struct {
  OttoCVariableKind kind;
  void *value;
} OttoCVariable;

typedef struct {
  uint_fast32_t length;
  OttoCVariable *array;
} OttoCArray;

OttoCVariable newOttoCString(const char *value) {
    char *allocatedString = (char *)GC_MALLOC(strlen((const char *)value) + 1); // +1 for null terminator

    if (allocatedString == NULL) {
      panic("Failed to allocate memory to string variable");
    }

    OttoCVariable result;
    strcpy(allocatedString, value);

    result.kind = OttoCVariableKindString;
    result.value = allocatedString;

    return result;
}

OttoCVariable newOttoCInteger(const int64_t value) {
    int64_t *allocatedInteger = (int64_t *)GC_MALLOC(sizeof(int64_t));

    if (allocatedInteger == NULL) {
      panic("Failed to allocate memory to integer variable");
    }

    *allocatedInteger = value;

    OttoCVariable result;
    result.kind = OttoCVariableKindInteger;
    result.value = allocatedInteger;

    return result;
}

OttoCVariable newOttoCBoolean(const bool value) {
  bool *allocatedBoolean = (bool *)GC_MALLOC(sizeof(bool));

  if (allocatedBoolean == NULL) {
    panic("Failed to allocate memory to integer variable");
  }

  *allocatedBoolean = value;

  OttoCVariable result;
  result.kind = OttoCVariableKindBoolean;
  result.value = allocatedBoolean;

  return result;
}

// OttoCVariable newOttoCArray(uint_fast32_t length, const OttoCVariable* value) {
//   OttoCArray *ottoCArray = (OttoCArray *)malloc(sizeof(ottoCArray));
//   OttoCArray *allocatedArray = (OttoCVariable *)malloc(length * sizeof(OttoCVariable));
//   ottoCArray->array = allocatedArray;
//   ottoCArray->length = length;
// 
//   memcpy(value, allocatedArray, length * sizeof(OttoCVariable));
// 
//   OttoCVariable result;
//   result.kind = OttoCVariableKindArray;
//   result.value = ottoCArray;
// 
//   return result;
// }

char *cString(OttoCVariable string) { return string.value; }
bool *cBool(OttoCVariable boolean) { return boolean.value; }
int64_t *cInt(OttoCVariable integer) { return integer.value; }

OttoCVariable function_int(OttoCVariable value) {
  switch (value.kind) {
  case OttoCVariableKindInteger:;
    return value;
  case OttoCVariableKindBoolean:;
    if (cBool(value)) {
      return newOttoCInteger(1LL);
    } else {
      return newOttoCInteger(0LL);
    }
  case OttoCVariableKindString:;
    return newOttoCInteger(strtoll(cString(value), NULL, 10));
  }
}

OttoCVariable function_string(OttoCVariable value) {
  switch (value.kind) {
  case OttoCVariableKindInteger:;
    char str[21];
    sprintf(str, "%" PRId64, *cInt(value));
    return newOttoCString(str);
  case OttoCVariableKindBoolean:;
    return *cBool(value) ? newOttoCString("true") : newOttoCString("false");
  case OttoCVariableKindString:;
    return value;
  }
}

OttoCVariable function_add(OttoCVariable first, OttoCVariable second) {
  return newOttoCInteger(*cInt(first) + *cInt(second));
}

OttoCVariable function_greaterThan(OttoCVariable first, OttoCVariable second) {
  return newOttoCBoolean(*cInt(first) > *cInt(second));
}

OttoCVariable function_lessThan(OttoCVariable first, OttoCVariable second) {
  return newOttoCBoolean(*cInt(first) < *cInt(second));
}

OttoCVariable function_equal(OttoCVariable first, OttoCVariable second) {
  return newOttoCBoolean(*cInt(first) == *cInt(second));
}

void function_print(OttoCVariable message) { printf("%s", cString(message)); }

void function_printLine(OttoCVariable message) {
  printf("%s\n", cString(message));
}

void function_fibonacci(OttoCVariable variable_limit) {
OttoCVariable variable_x = newOttoCInteger(0LL);
OttoCVariable variable_y = newOttoCInteger(1LL);
OttoCVariable variable_z = newOttoCInteger(0LL);
while (*cBool(function_lessThan(variable_x, variable_limit))) {
function_printLine(function_string(variable_x));
variable_z = function_add(variable_x, variable_y);
variable_x = variable_y;
variable_y = variable_z;
}}
void function_main() {
function_fibonacci(newOttoCInteger(3000000000000000000LL));
}//go:build ignore

int main() {
  GC_INIT();
  function_main();
  return 0;
}
