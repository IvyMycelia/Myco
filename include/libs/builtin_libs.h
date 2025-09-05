#ifndef MYCO_BUILTIN_LIBS_H
#define MYCO_BUILTIN_LIBS_H

#include "../core/interpreter.h"

// Include all library headers
#include "math.h"
#include "string.h"
#include "array.h"
#include "file.h"
#include "dir.h"
#include "maps.h"
#include "sets.h"
#include "trees.h"
#include "graphs.h"
#include "heaps.h"
#include "queues.h"
#include "stacks.h"
#include "time.h"
#include "regex.h"
#include "json.h"
#include "http.h"

// Register all built-in libraries
void register_all_builtin_libraries(Interpreter* interpreter);

#endif // MYCO_BUILTIN_LIBS_H
