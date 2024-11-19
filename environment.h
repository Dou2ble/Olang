#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "value.h"

// TODO: q: is this too small?
#define ENVIRONMENT_SIZE 10000

typedef struct EnvironmentEntry {
  char *key;
  Value value;
  struct EnvironmentEntry *next;
} EnvEntry;

typedef struct Environment {
  struct Environment *outer;
  EnvEntry **entries;
} Environment;

Environment newEnvironment(Environment *outer);

// assumes that the key is allocated on the heap
void environmentSet(Environment *env, char *key, Value value);

// returns newNullValue if key was not found
Value environmentGet(Environment *env, char *key);

#endif // !ENVIRONMENT_H
