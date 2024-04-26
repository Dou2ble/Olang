//go:build ignore

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { PlispVariableKindString } PlispVariableKind;

typedef struct {
  PlispVariableKind kind;
  void *value;
} PlispVariable;

PlispVariable* newPlispVariable(const PlispVariableKind kind, const void *value) {
  PlispVariable* result = (PlispVariable*)malloc(sizeof(PlispVariable));

  switch (kind) {
  case PlispVariableKindString:;
    char *allocatedString = (char *)malloc(strlen((const char*)value) + 1); // +1 for null terminator
    if (allocatedString != NULL ) {
      strcpy(allocatedString, value);
      result->kind = PlispVariableKindString;
      result->value = allocatedString;
    } else {
      return NULL;
    }
    return result;
  }

  return NULL;
};

const char* cString(PlispVariable *string) {
  return string->value;
}

void function_print(PlispVariable *message) {
  printf("%s", cString(message));
}

void function_printLine(PlispVariable *message) {
  printf("%s\n", cString(message));
}

int main() {
  PlispVariable* myString = newPlispVariable(PlispVariableKindString, "Hello, World");
  function_printLine(myString);

  return 0;
}
