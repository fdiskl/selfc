#ifndef ASCC_STRING_H
#define ASCC_STRING_H

// string type should be used only as read-only type
typedef const char *string;

// Returns a read-only string stored in a global string arena.
// The returned string must not be freed or modified.
// Valid until program end or global string arena is reset.
string new_string(const char *original_string);

#endif
