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

OttoCVariable function_string(OttoCVariable value);

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

  allocatedOttoCArray->length = length;
  allocatedOttoCArray->array = heapArray;

  OttoCVariable result;
  result.kind = OttoCVariableKindArray;
  result.value = allocatedOttoCArray;
  return result;
}

char *cString(OttoCVariable string) { return string.value; }
bool *cBool(OttoCVariable boolean) { return boolean.value; }
int64_t *cInt(OttoCVariable integer) { return integer.value; }
OttoCVariable *cArray(OttoCVariable array) { return ((OttoCArray *)array.value)->array; }

OttoCVariable function_readFile(OttoCVariable path) {
  FILE *filePointer = fopen(cString(path), "r");
  if (filePointer == NULL) {
    panic("Failed to open file");
  }

  fseek(filePointer, 0, SEEK_END);
  int fileSize = ftell(filePointer);
  fseek(filePointer, 0, SEEK_SET);

  char *fileContent = (char *)GC_MALLOC(fileSize*sizeof(char));
  if (fileContent == NULL) {
    panic("Failed to allocated memory while reading file");
  }

  fread(fileContent, sizeof(char), fileSize, filePointer);
  fileContent[fileSize] = '\0';
  fclose(filePointer);

  return newOttoCString(fileContent);
}

OttoCVariable function_len(OttoCVariable value) {
  if (value.kind == OttoCVariableKindArray) {
    return newOttoCInteger(((OttoCArray *)value.value)->length);
  }

  OttoCVariable string = function_string(value);
  return newOttoCInteger(strlen(cString(string)));
}

OttoCVariable dupe(OttoCVariable value) {
  switch (value.kind) {
  case OttoCVariableKindBoolean:;
    return newOttoCBoolean(*cBool(value));
  case OttoCVariableKindInteger:;
    return newOttoCInteger(*cInt(value));
  case OttoCVariableKindString:;
    return newOttoCBoolean(*cString(value));
  case OttoCVariableKindArray:;
    return newOttoCArray((int_fast32_t)*cInt(function_len(value)), cArray(value));
  }
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

OttoCVariable function_getIndex(OttoCVariable value, OttoCVariable index) {
  if (value.kind == OttoCVariableKindArray) {
    return ((OttoCArray *)value.value)->array[*cInt(index)];
  }

  OttoCVariable string = function_string(value);
  char c = cString(string)[*cInt(index)];
  char s[2] = {c, '\0'};
  return newOttoCString(s);
}

OttoCVariable function_setIndex(OttoCVariable array, OttoCVariable index, OttoCVariable value) {
  ((OttoCArray *)array.value)->array[*cInt(index)] = value;
  return array;
}

OttoCVariable function_append(OttoCVariable array, OttoCVariable appendage) {
  int_fast32_t length = ((OttoCArray *)array.value)->length;

  OttoCVariable arrayButLonger[length + 1];
  arrayButLonger[0] = newOttoCInteger(0);
  memcpy(arrayButLonger, ((OttoCArray *)array.value)->array, ((OttoCArray *)array.value)->length*sizeof(OttoCVariable));
  
  OttoCVariable result = newOttoCArray(length + 1, arrayButLonger);
  ((OttoCArray *)result.value)->array[length] = appendage;
  return result;
}


OttoCVariable function_add(OttoCVariable first, OttoCVariable second) {
  return newOttoCInteger(*cInt(first) + *cInt(second));
}

OttoCVariable function_subtract(OttoCVariable first, OttoCVariable second) {
  return newOttoCInteger(*cInt(first) - *cInt(second));
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
