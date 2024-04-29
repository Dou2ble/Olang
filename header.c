//go:build ignore

#include <gc/gc.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <gc.h>

void panic(char *message) { printf("==> OttoC Panic: %s", message); }

typedef enum {
  OttoCVariableKindString,
  OttoCVariableKindInteger
} OttoCVariableKind;

typedef struct {
  OttoCVariableKind kind;
  void *value;
} OttoCVariable;

OttoCVariable newOttoCVariable(const OttoCVariableKind kind,
                               const void *value) {
  OttoCVariable result;

  switch (kind) {
  case OttoCVariableKindString:;
    char *allocatedString = (char *)GC_MALLOC(strlen((const char *)value) +
                                              1); // +1 for null terminator
    if (allocatedString != NULL) {
      strcpy(allocatedString, value);
      result.kind = OttoCVariableKindString;
      result.value = allocatedString;
    } else {
      panic("Failed to allocate memory to variable");
    }

  case OttoCVariableKindInteger:;
    int64_t *allocatedInteger = (int64_t *)GC_MALLOC(sizeof(int64_t));
    if (allocatedString != NULL) {
      allocatedInteger = (int64_t *)value;
      result.kind = OttoCVariableKindInteger;
      result.value = allocatedInteger;
    }
  }

  return result;
};

const char *cString(OttoCVariable string) { return string.value; }

void function_print(OttoCVariable message) { printf("%s", cString(message)); }

void function_printLine(OttoCVariable *message) {
  printf("%s\n", cString(message));
}
