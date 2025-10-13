/**
 * @file nan_boxing.c
 * @brief NaN-boxing implementation for efficient value representation
 */

#include "../../include/core/optimization/nan_boxing.h"
#include "../../include/core/interpreter/value_operations.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================================
// NaN-BOXING CONSTANTS AND UTILITIES
// ============================================================================

// IEEE 754 double precision NaN representation
#define NAN_BOX_QUIET_NAN 0x7FF8000000000000ULL
#define NAN_BOX_SIGNALING_NAN 0x7FF0000000000001ULL

// Tag masks and shifts
#define NAN_BOX_TAG_MASK 0xFFFF000000000000ULL
#define NAN_BOX_PAYLOAD_MASK 0x0000FFFFFFFFFFFFULL
#define NAN_BOX_TAG_SHIFT 48

// Special values
#define NAN_BOX_NULL_VALUE 0x7FF3000000000000ULL
#define NAN_BOX_UNDEFINED_VALUE 0x7FF4000000000000ULL
#define NAN_BOX_TRUE_VALUE 0x7FF2000000000001ULL
#define NAN_BOX_FALSE_VALUE 0x7FF2000000000000ULL

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

NanBoxingContext* nan_boxing_create(void) {
    NanBoxingContext* context = malloc(sizeof(NanBoxingContext));
    if (!context) {
        return NULL;
    }
    
    context->is_enabled = 1;
    context->conversion_count = 0;
    context->memory_saved = 0;
    
    return context;
}

void nan_boxing_free(NanBoxingContext* context) {
    if (context) {
        free(context);
    }
}

// ============================================================================
// CONVERSION FUNCTIONS
// ============================================================================

NanBoxedValue nan_boxing_from_value(NanBoxingContext* context, const Value* value) {
    if (!context || !value) {
        return NAN_BOX_NULL_VALUE;
    }
    
    context->conversion_count++;
    
    switch (value->type) {
        case VALUE_NUMBER: {
            double num = value->data.number_value;
            // Check if it's a valid number (not NaN or infinity)
            if (isnan(num) || isinf(num)) {
                return NAN_BOX_NULL_VALUE;
            }
            // Check if it's an integer that fits in 32 bits
            if (num == (int32_t)num && num >= INT32_MIN && num <= INT32_MAX) {
                return nan_boxing_create_integer((int32_t)num);
            }
            // Store as double
            return nan_boxing_create_number(num);
        }
        
        case VALUE_STRING:
            return nan_boxing_create_string(value->data.string_value);
            
        case VALUE_BOOLEAN:
            return nan_boxing_create_boolean(value->data.boolean_value);
            
        case VALUE_NULL:
            return nan_boxing_create_null();
            
        case VALUE_ARRAY:
            return nan_boxing_create_array((Value**)value->data.array_value.elements);
            
        case VALUE_OBJECT:
            return nan_boxing_create_object(value->data.object_value.values);
            
        case VALUE_FUNCTION:
            return nan_boxing_create_function(value->data.function_value.body);
            
        default:
            return NAN_BOX_NULL_VALUE;
    }
}

Value* nan_boxing_to_value(NanBoxingContext* context, NanBoxedValue nan_boxed) {
    if (!context) {
        return NULL;
    }
    
    context->conversion_count++;
    
    Value* value = malloc(sizeof(Value));
    if (!value) {
        return NULL;
    }
    
    // Initialize value
    value->ref_count = 1;
    
    if (nan_boxing_is_number(nan_boxed)) {
        value->type = VALUE_NUMBER;
        value->data.number_value = nan_boxing_get_number(nan_boxed);
    } else if (nan_boxing_is_integer(nan_boxed)) {
        value->type = VALUE_NUMBER;
        value->data.number_value = (double)nan_boxing_get_integer(nan_boxed);
    } else if (nan_boxing_is_boolean(nan_boxed)) {
        value->type = VALUE_BOOLEAN;
        value->data.boolean_value = nan_boxing_get_boolean(nan_boxed);
    } else if (nan_boxing_is_null(nan_boxed)) {
        value->type = VALUE_NULL;
    } else if (nan_boxing_is_string(nan_boxed)) {
        value->type = VALUE_STRING;
        value->data.string_value = nan_boxing_get_string(nan_boxed);
    } else if (nan_boxing_is_array(nan_boxed)) {
        value->type = VALUE_ARRAY;
        value->data.array_value.elements = (void**)nan_boxing_get_array(nan_boxed);
        value->data.array_value.count = 0; // Would need to track this separately
        value->data.array_value.capacity = 0;
    } else if (nan_boxing_is_object(nan_boxed)) {
        value->type = VALUE_OBJECT;
        value->data.object_value.values = nan_boxing_get_object(nan_boxed);
        value->data.object_value.count = 0;
        value->data.object_value.capacity = 0;
    } else if (nan_boxing_is_function(nan_boxed)) {
        value->type = VALUE_FUNCTION;
        value->data.function_value.body = nan_boxing_get_function(nan_boxed);
    } else {
        value->type = VALUE_NULL;
    }
    
    return value;
}

// ============================================================================
// TYPE CHECKING FUNCTIONS
// ============================================================================

ValueType nan_boxing_get_type(NanBoxedValue nan_boxed) {
    if (nan_boxing_is_number(nan_boxed)) {
        return VALUE_NUMBER;
    } else if (nan_boxing_is_integer(nan_boxed)) {
        return VALUE_NUMBER;
    } else if (nan_boxing_is_boolean(nan_boxed)) {
        return VALUE_BOOLEAN;
    } else if (nan_boxing_is_null(nan_boxed)) {
        return VALUE_NULL;
    } else if (nan_boxing_is_string(nan_boxed)) {
        return VALUE_STRING;
    } else if (nan_boxing_is_array(nan_boxed)) {
        return VALUE_ARRAY;
    } else if (nan_boxing_is_object(nan_boxed)) {
        return VALUE_OBJECT;
    } else if (nan_boxing_is_function(nan_boxed)) {
        return VALUE_FUNCTION;
    } else {
        return VALUE_NULL;
    }
}

int nan_boxing_is_number(NanBoxedValue nan_boxed) {
    // Check if it's a valid IEEE 754 number (not NaN)
    return !isnan(*(double*)&nan_boxed) && !isinf(*(double*)&nan_boxed);
}

int nan_boxing_is_integer(NanBoxedValue nan_boxed) {
    return (nan_boxed & NAN_BOX_TAG_MASK) == (NAN_BOX_TAG_INTEGER << NAN_BOX_TAG_SHIFT);
}

int nan_boxing_is_boolean(NanBoxedValue nan_boxed) {
    return (nan_boxed & NAN_BOX_TAG_MASK) == (NAN_BOX_TAG_BOOLEAN << NAN_BOX_TAG_SHIFT);
}

int nan_boxing_is_null(NanBoxedValue nan_boxed) {
    return nan_boxed == NAN_BOX_NULL_VALUE;
}

int nan_boxing_is_string(NanBoxedValue nan_boxed) {
    return (nan_boxed & NAN_BOX_TAG_MASK) == (NAN_BOX_TAG_STRING << NAN_BOX_TAG_SHIFT);
}

int nan_boxing_is_array(NanBoxedValue nan_boxed) {
    return (nan_boxed & NAN_BOX_TAG_MASK) == (NAN_BOX_TAG_ARRAY << NAN_BOX_TAG_SHIFT);
}

int nan_boxing_is_object(NanBoxedValue nan_boxed) {
    return (nan_boxed & NAN_BOX_TAG_MASK) == (NAN_BOX_TAG_OBJECT << NAN_BOX_TAG_SHIFT);
}

int nan_boxing_is_function(NanBoxedValue nan_boxed) {
    return (nan_boxed & NAN_BOX_TAG_MASK) == (NAN_BOX_TAG_FUNCTION << NAN_BOX_TAG_SHIFT);
}

// ============================================================================
// VALUE EXTRACTION FUNCTIONS
// ============================================================================

double nan_boxing_get_number(NanBoxedValue nan_boxed) {
    return *(double*)&nan_boxed;
}

int32_t nan_boxing_get_integer(NanBoxedValue nan_boxed) {
    return (int32_t)(nan_boxed & NAN_BOX_PAYLOAD_MASK);
}

int nan_boxing_get_boolean(NanBoxedValue nan_boxed) {
    return (int)(nan_boxed & NAN_BOX_PAYLOAD_MASK);
}

char* nan_boxing_get_string(NanBoxedValue nan_boxed) {
    return (char*)(nan_boxed & NAN_BOX_PAYLOAD_MASK);
}

Value** nan_boxing_get_array(NanBoxedValue nan_boxed) {
    return (Value**)(nan_boxed & NAN_BOX_PAYLOAD_MASK);
}

void* nan_boxing_get_object(NanBoxedValue nan_boxed) {
    return (void*)(nan_boxed & NAN_BOX_PAYLOAD_MASK);
}

void* nan_boxing_get_function(NanBoxedValue nan_boxed) {
    return (void*)(nan_boxed & NAN_BOX_PAYLOAD_MASK);
}

// ============================================================================
// VALUE CREATION FUNCTIONS
// ============================================================================

NanBoxedValue nan_boxing_create_number(double number) {
    return *(NanBoxedValue*)&number;
}

NanBoxedValue nan_boxing_create_integer(int32_t integer) {
    return NAN_BOX_QUIET_NAN | (NAN_BOX_TAG_INTEGER << NAN_BOX_TAG_SHIFT) | (integer & NAN_BOX_PAYLOAD_MASK);
}

NanBoxedValue nan_boxing_create_boolean(int boolean) {
    return NAN_BOX_QUIET_NAN | (NAN_BOX_TAG_BOOLEAN << NAN_BOX_TAG_SHIFT) | (boolean & NAN_BOX_PAYLOAD_MASK);
}

NanBoxedValue nan_boxing_create_null(void) {
    return NAN_BOX_NULL_VALUE;
}

NanBoxedValue nan_boxing_create_string(char* string) {
    return NAN_BOX_QUIET_NAN | (NAN_BOX_TAG_STRING << NAN_BOX_TAG_SHIFT) | ((uintptr_t)string & NAN_BOX_PAYLOAD_MASK);
}

NanBoxedValue nan_boxing_create_array(Value** array) {
    return NAN_BOX_QUIET_NAN | (NAN_BOX_TAG_ARRAY << NAN_BOX_TAG_SHIFT) | ((uintptr_t)array & NAN_BOX_PAYLOAD_MASK);
}

NanBoxedValue nan_boxing_create_object(void* object) {
    return NAN_BOX_QUIET_NAN | (NAN_BOX_TAG_OBJECT << NAN_BOX_TAG_SHIFT) | ((uintptr_t)object & NAN_BOX_PAYLOAD_MASK);
}

NanBoxedValue nan_boxing_create_function(void* function) {
    return NAN_BOX_QUIET_NAN | (NAN_BOX_TAG_FUNCTION << NAN_BOX_TAG_SHIFT) | ((uintptr_t)function & NAN_BOX_PAYLOAD_MASK);
}

// ============================================================================
// STATISTICS
// ============================================================================

void nan_boxing_get_stats(NanBoxingContext* context,
                          size_t* conversion_count,
                          size_t* memory_saved) {
    if (!context || !conversion_count || !memory_saved) {
        return;
    }
    
    *conversion_count = context->conversion_count;
    *memory_saved = context->memory_saved;
}
