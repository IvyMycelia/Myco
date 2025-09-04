#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "interpreter.h"

// Environment management functions
Environment* environment_create(Environment* parent);
void environment_free(Environment* env);
void environment_define(Environment* env, const char* name, Value value);
Environment* environment_copy(Environment* env);
Value environment_get(Environment* env, const char* name);
void environment_assign(Environment* env, const char* name, Value value);
int environment_exists(Environment* env, const char* name);

#endif // ENVIRONMENT_H
