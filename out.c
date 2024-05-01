//go:build ignore

#include <gc/gc.h>
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

OttoCVariable newEmptyOttoCArray() {
  OttoCArray *allocatedOttoCArray = (OttoCArray *)GC_MALLOC(sizeof(OttoCArray));

  if (allocatedOttoCArray == NULL) {
    panic("Failed to allocate memory to array variable");
  }

  allocatedOttoCArray->length = 0;
  allocatedOttoCArray->array = NULL;

  OttoCVariable result;
  result.kind = OttoCVariableKindArray;
  result.value = allocatedOttoCArray;
  return result;
}

OttoCVariable newOttoCArray(int_fast32_t length, OttoCVariable *array) {
  OttoCVariable *heapArray = (OttoCVariable *)GC_MALLOC(length*sizeof(OttoCVariable));

  if (heapArray == NULL) {
    panic("Failed to allocate memory to array variable");
  }

  OttoCArray *allocatedOttoCArray = (OttoCArray *)GC_MALLOC(sizeof(OttoCArray));

  if (allocatedOttoCArray == NULL) {
    panic("Failed to allocate memory to array variable");
  }

  memcpy(heapArray, array, length*sizeof(OttoCVariable));

  allocatedOttoCArray->length = 0;
  allocatedOttoCArray->array = NULL;

  OttoCVariable result;
  result.kind = OttoCVariableKindArray;
  result.value = allocatedOttoCArray;
  return result;
}

char *cString(OttoCVariable string) { return string.value; }
bool *cBool(OttoCVariable boolean) { return boolean.value; }
int64_t *cInt(OttoCVariable integer) { return integer.value; }

OttoCVariable function_append(OttoCVariable array, OttoCVariable appendage) {
  printf("1\n");
  int_fast32_t length = ((OttoCArray *)array.value)->length;
  printf("2\n");

  OttoCVariable arrayButLonger[length + 1];
  arrayButLonger[0] = newOttoCInteger(0);
  // memcpy(arrayButLonger, ((OttoCArray *)array.value)->array, ((OttoCArray *)array.value)->length*sizeof(OttoCVariable));
  
  OttoCVariable result = newOttoCArray(length + 1, arrayButLonger);
  printf("3\n");
  printf("%ld\n", length);
  ((OttoCArray *)result.value)->array[length] = appendage;
  printf("4\n");
  return result;
}

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
  case OttoCVariableKindArray:;
    return newOttoCInteger(((OttoCArray *)value.value)->length);
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
  case OttoCVariableKindArray:;
    return function_string(function_int(value));
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

void function_main() {
OttoCVariable variable_array = newEmptyOttoCArray();
variable_array = function_append(variable_array, newOttoCInteger(1LL));
function_printLine(function_string(variable_array));
}//go:build ignore

int main() {
  GC_INIT();
  function_main();
  return 0;
}
