#include "codegen_headers.h"
#include "codegen_utils.h"
#include <stdio.h>
#include <string.h>

// Generate C headers
int codegen_generate_c_headers(CodeGenContext* context) {
    if (!context) return 0;
    
    // Standard includes
    codegen_write_string(context, "#include <stdio.h>\n");
    codegen_write_string(context, "#include <stdlib.h>\n");
    codegen_write_string(context, "#include <string.h>\n");
    codegen_write_string(context, "#include <math.h>\n");
    codegen_write_string(context, "#include <time.h>\n");
    codegen_write_string(context, "#include <regex.h>\n");
    codegen_write_string(context, "#include <curl/curl.h>\n");
    codegen_write_string(context, "#include <microhttpd.h>\n");
    codegen_write_string(context, "#include <zlib.h>\n");
    codegen_write_string(context, "\n");
    
    // Myco runtime includes
    codegen_write_string(context, "#include \"myco_runtime.h\"\n");
    codegen_write_string(context, "\n");
    
    // Type definitions
    codegen_generate_c_type_definitions(context);
    
    // Function declarations
    codegen_generate_c_function_declarations(context);
    
    // Library functions
    codegen_generate_c_library_functions(context);
    
    return 1;
}

// Generate type definitions
int codegen_generate_c_type_definitions(CodeGenContext* context) {
    if (!context) return 0;
    
    // HttpResponse struct
    codegen_write_string(context, "typedef struct {\n");
    codegen_write_string(context, "    int status_code;\n");
    codegen_write_string(context, "    char* type;\n");
    codegen_write_string(context, "    char* status_text;\n");
    codegen_write_string(context, "    char* body;\n");
    codegen_write_string(context, "    int success;\n");
    codegen_write_string(context, "} HttpResponse;\n");
    codegen_write_string(context, "\n");
    
    return 1;
}

// Generate function declarations
int codegen_generate_c_function_declarations(CodeGenContext* context) {
    if (!context) return 0;
    
    // Placeholder function declarations
    codegen_write_string(context, "// Placeholder function declarations\n");
    codegen_write_string(context, "char* placeholder_greet(void);\n");
    codegen_write_string(context, "double placeholder_getValue(void);\n");
    codegen_write_string(context, "double placeholder_increment(void);\n");
    codegen_write_string(context, "char* placeholder_getName(void);\n");
    codegen_write_string(context, "double placeholder_process(void);\n");
    codegen_write_string(context, "double placeholder_calculate(void);\n");
    codegen_write_string(context, "char* placeholder_speak(void);\n");
    codegen_write_string(context, "void* placeholder_lambda(void);\n");
    codegen_write_string(context, "\n");
    
    return 1;
}

// Generate library functions
int codegen_generate_c_library_functions(CodeGenContext* context) {
    if (!context) return 0;
    
    // Placeholder function implementations
    codegen_write_string(context, "// Placeholder function implementations\n");
    codegen_write_string(context, "char* placeholder_greet(void) { return \"Hello, World\"; }\n");
    codegen_write_string(context, "double placeholder_getValue(void) { return 42.0; }\n");
    codegen_write_string(context, "double placeholder_increment(void) { return 1.0; }\n");
    codegen_write_string(context, "char* placeholder_getName(void) { return \"TestName\"; }\n");
    codegen_write_string(context, "double placeholder_process(void) { return 100.0; }\n");
    codegen_write_string(context, "double placeholder_calculate(void) { return 3.14159; }\n");
    codegen_write_string(context, "char* placeholder_speak(void) { return \"Woof!\"; }\n");
    codegen_write_string(context, "void* placeholder_lambda(void) { return NULL; }\n");
    codegen_write_string(context, "\n");
    
    return 1;
}

// Generate stdlib headers
int codegen_generate_c_stdlib_headers(CodeGenContext* context) {
    if (!context) return 0;
    
    // Standard library headers
    codegen_write_string(context, "#include <stdio.h>\n");
    codegen_write_string(context, "#include <stdlib.h>\n");
    codegen_write_string(context, "#include <string.h>\n");
    codegen_write_string(context, "#include <math.h>\n");
    codegen_write_string(context, "#include <time.h>\n");
    codegen_write_string(context, "#include <regex.h>\n");
    codegen_write_string(context, "#include <curl/curl.h>\n");
    codegen_write_string(context, "#include <microhttpd.h>\n");
    codegen_write_string(context, "#include <zlib.h>\n");
    codegen_write_string(context, "\n");
    
    return 1;
}

// Generate library includes
int codegen_generate_c_library_includes(CodeGenContext* context) {
    if (!context) return 0;
    
    // Library includes
    codegen_write_string(context, "#include \"myco_runtime.h\"\n");
    codegen_write_string(context, "\n");
    
    return 1;
}
