//go:build ignore

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gc.h>

void panic(char *message) { printf("==> OttoC Panic: %s", message); }

typedef enum {
  OttoCVariableKindString,
  OttoCVariableKindInteger,
  OttoCVariableKindBoolean
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
      panic("Failed to allocate memory to string variable");
    }

  case OttoCVariableKindInteger:;
    int64_t *allocatedInteger = (int64_t *)GC_MALLOC(sizeof(int64_t));
    if (allocatedString != NULL) {
      allocatedInteger = (int64_t *)value;
      result.kind = OttoCVariableKindInteger;
      result.value = allocatedInteger;
    } else {
      panic("Failed to allocate memory to integer variable");
    }

  case OttoCVariableKindBoolean:;
    bool *allocatedBoolean = (bool *)GC_MALLOC(sizeof(bool));
    if (allocatedBoolean != NULL) {
      allocatedBoolean = (bool *)value;
      result.kind = OttoCVariableKindBoolean;
      result.value = allocatedBoolean;
    } else {
      panic("Failed to allocate memory to boolean variable");
    }

  return result;
  };
}

char *cString(OttoCVariable string) { return string.value; }
bool *cBool(OttoCVariable boolean) { return boolean.value; }
int64_t *cInt(OttoCVariable integer) { return integer.value; }
// OttoCVariable add(OttoCVariable first, OttoCVariable second) {
//   return newOttoCVariable(OttoCVariableKindInteger, (void *)(cInt(first) + cInt(second)));
// };

OttoCVariable string(OttoCVariable s) {
  switch (s.kind) {
  case OttoCVariableKindString:;
    return s;
  case OttoCVariableKindInteger:;
    return newOttoCVariable(OttoCVariableKindString, (void *)sprintf("%d", cInt(s)));
  }
}

void function_print(OttoCVariable message) { printf("%s", cString(message)); }

void function_printLine(OttoCVariable message) {
  printf("%s\n", cString(message));
}

void function_main() {
function_printLine(newOttoCVariable(OttoCVariableKindInteger, (void *)213LL));
}//go:build ignore

int main() {
  GC_INIT();
  function_main();
  return 0;
}
