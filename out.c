//go:build ignore

#include <gc/gc.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <gc.h>

typedef enum { OttoCVariableKindString OttoCVariableKindInteger } OttoCVariableKind;

typedef struct {
  OttoCVariableKind kind;
  void *value;
} OttoCVariable;

OttoCVariable *newOttoCVariable(const OttoCVariableKind kind,
                                const void *value) {
  OttoCVariable *result = (OttoCVariable *)GC_MALLOC(sizeof(OttoCVariable));

  switch (kind) {
  case OttoCVariableKindString:;
    char *allocatedString = (char *)GC_MALLOC(strlen((const char*)value) + 1); // +1 for null terminator
    if (allocatedString != NULL ) {
      strcpy(allocatedString, value);
      result->kind = OttoCVariableKindString;
      result->value = allocatedString;
    } else {
      return NULL;
    }

    return result;

  case OttoCVariableKindInteger:;
    int64_t *allocatedInteger = (int64_t *)GC_MALLOC(sizeof(int64_t));
    if (allocatedString != NULL ) {
      allocatedInteger = (int64_t *)value;
      result->kind = OttoCVariableKindInteger;
      result->value = allocatedInteger;
    } else {
      return NULL;
    }

    return result; 
  }

  return NULL;
};

const char *cString(OttoCVariable *string) { return string->value; }

void function_print(OttoCVariable *message) { printf("%s", cString(message)); }

void function_printLine(OttoCVariable *message) {
  printf("%s\n", cString(message));
}

void function_sendMessage(OttoCVariable *variable_message) {
function_print(newOttoCVariable(OttoCVariableKindString, "Message: "));
function_printLine(variable_message);

};

void function_welcome() {
function_sendMessage(newOttoCVariable(OttoCVariableKindString, "Welcome!"));

};

void function_main() {
function_welcome();

};
//go:build ignore

int main() {
  GC_INIT();
  function_main();
  return 0;
}
