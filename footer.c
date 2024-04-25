//go:build ignore

typedef enum { PlispVariableKindString } PlispVariableKind;

typedef struct {
  PlispVariableKind kind;
  void *value;
} PlispVariable;

PlispVariable newPlispVariable(PlispVariableKind kind, void *value) {
  PlispVariable result;

  result.kind = kind;
  result.value = value;

  return result;
};

