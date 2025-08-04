#include "string.h"
#include "arena.h"
#include <assert.h>
#include <string.h>

arena *string_arena;

string new_string(const char *original_string) {
  size_t len = strlen(original_string);
  char *dest = ARENA_ALLOC_ARRAY(string_arena, char, len + 1); // +1 for '\0'
  assert(dest);
  memcpy(dest, original_string, len);
  dest[len] = '\0';
  return dest;
}
