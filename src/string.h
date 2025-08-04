#ifndef ASCC_STRING_H
#define ASCC_STRING_H

// string type should be used only as read-only type
#include "arena.h"
typedef const char *string;

// global string arena, should be initialized before any 'new_string' invocation
extern arena string_arena;

// Returns a read-only string stored in a global string arena.
// The returned string must not be freed or modified.
// Valid until program end or global string arena is reset.
string new_string(const char *original_string);

#endif
