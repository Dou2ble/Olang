#ifndef SLOG_H
#define SLOG_H

#include <stddef.h>

typedef enum { WARNING, ERROR } SlogLevel;

typedef struct {
  size_t start;
  size_t end;
} Region;

void slogLocation(const char *source, size_t location, SlogLevel level,
                  char *message);
void slogRegion(const char *source, Region region, SlogLevel level,
                char *message);
void slog(SlogLevel level, char *message);

#endif // !SLOG_H
