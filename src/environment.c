#include "environment.h"
#include "value.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t hash(const char *key) {
  size_t value = 0;
  size_t keyLength = strlen(key);

  for (size_t i = 0; i < keyLength; i++) {
    value = value * 37 + key[i];
  }

  return value % ENVIRONMENT_SIZE;
}

Environment newEnvironment(Environment *outer) {
  Environment env;
  env.outer = outer;

  // allocate the table entries
  env.entries = malloc(sizeof(EnvEntry *) * ENVIRONMENT_SIZE);
  for (size_t i = 0; i < ENVIRONMENT_SIZE; i++) {
    env.entries[i] = NULL;
  }

  return env;
}

// assumes that the key is allocated on the heap
EnvEntry *newEnvEntry(char *key, Value value) {
  EnvEntry *entry = malloc(sizeof(EnvEntry));

  entry->key = key;
  entry->value = value;
  entry->next = NULL;

  return entry;
}

// assumes that the key is allocated on the heap
void environmentSet(Environment *env, char *key, Value value) {
  size_t slot = hash(key);

  // check if the entry at that slot is set
  EnvEntry *entry = env->entries[slot];
  if (entry == NULL) {
    // check if it exsists in the outer environment
    if (env->outer != NULL &&
        environmentGet(env->outer, key).type != VALUE_NULL) {
      environmentSet(env->outer, key, value);
      return;
    }

    env->entries[slot] = newEnvEntry(key, value);
    return;
  }

  EnvEntry *previousEntry;
  // walk through each entry until either the end is reached or a matching key
  // if found
  while (entry != NULL) {
    if (strcmp(key, entry->key) == 0) {
      // a match was found! now we will need to replace the value
      entry->value = value;
      return;
    }

    // go to next
    previousEntry = entry;
    entry = previousEntry->next;
  }

  // we reached the end of the chain, add a new entry
  previousEntry->next = newEnvEntry(key, value);
}

// returns newNullValue if key was not found
Value environmentGet(Environment *env, char *key) {
  size_t slot = hash(key);

  EnvEntry *entry = env->entries[slot];

  // go through each entry in the slot which could just be a single thing
  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      return entry->value;
    }

    entry = entry->next;
  }

  // if we reach here then no matching key was found, that means that the value
  // is not present in this environment, however it might be in the outer
  // environment let's check that
  if (env->outer != NULL) {
    return environmentGet(env->outer, key);
  }

  // if we have come this far then everything has failed and we will need to
  // return null
  return newNullValue();
}
