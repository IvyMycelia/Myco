/**
 * @file nan_boxing.h
 * @brief NaN-boxing for efficient value representation
 * 
 * Uses NaN-boxing to store tagged values in 8 bytes, eliminating pointer
 * indirection and reducing memory overhead for zero-copy optimization.
 */

#ifndef MYCO_NAN_BOXING_H
#define MYCO_NAN_BOXING_H

#include "../interpreter/value_operations.h"
#include <stdint.h>
#include <stddef.h>

/**
 * @brief NaN-boxed value (8 bytes)
 */
typedef uint64_t NanBoxedValue;

/**
 * @brief Value type tags for NaN-boxing
 */
typedef enum {
    NAN_BOX_TAG_NUMBER = 0x0000,      // IEEE 754 number
    NAN_BOX_TAG_INTEGER = 0x1000,     // 32-bit integer
    NAN_BOX_TAG_BOOLEAN = 0x2000,     // Boolean value
    NAN_BOX_TAG_NULL = 0x3000,        // Null value
    NAN_BOX_TAG_UNDEFINED = 0x4000,   // Undefined value
    NAN_BOX_TAG_STRING = 0x5000,      // String pointer
    NAN_BOX_TAG_ARRAY = 0x6000,       // Array pointer
    NAN_BOX_TAG_OBJECT = 0x7000,      // Object pointer
    NAN_BOX_TAG_FUNCTION = 0x8000,    // Function pointer
    NAN_BOX_TAG_SPECIAL = 0x9000      // Special values
} NanBoxTag;

/**
 * @brief NaN-boxing context
 */
typedef struct {
    int is_enabled;                   // Whether NaN-boxing is enabled
    size_t conversion_count;          // Number of conversions performed
    size_t memory_saved;              // Memory saved by NaN-boxing
} NanBoxingContext;

/**
 * @brief Create NaN-boxing context
 * 
 * @return NanBoxingContext* New context or NULL on failure
 */
NanBoxingContext* nan_boxing_create(void);

/**
 * @brief Free NaN-boxing context
 * 
 * @param context Context to free
 */
void nan_boxing_free(NanBoxingContext* context);

/**
 * @brief Convert Value to NaN-boxed representation
 * 
 * @param context NaN-boxing context
 * @param value Value to convert
 * @return NanBoxedValue NaN-boxed value
 */
NanBoxedValue nan_boxing_from_value(NanBoxingContext* context, const Value* value);

/**
 * @brief Convert NaN-boxed value to Value
 * 
 * @param context NaN-boxing context
 * @param nan_boxed NaN-boxed value
 * @return Value* Converted value or NULL on failure
 */
Value* nan_boxing_to_value(NanBoxingContext* context, NanBoxedValue nan_boxed);

/**
 * @brief Get type of NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return ValueType Type of the value
 */
ValueType nan_boxing_get_type(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is a number
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if number, 0 otherwise
 */
int nan_boxing_is_number(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is an integer
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if integer, 0 otherwise
 */
int nan_boxing_is_integer(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is a boolean
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if boolean, 0 otherwise
 */
int nan_boxing_is_boolean(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is null
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if null, 0 otherwise
 */
int nan_boxing_is_null(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is a string
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if string, 0 otherwise
 */
int nan_boxing_is_string(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is an array
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if array, 0 otherwise
 */
int nan_boxing_is_array(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is an object
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if object, 0 otherwise
 */
int nan_boxing_is_object(NanBoxedValue nan_boxed);

/**
 * @brief Check if NaN-boxed value is a function
 * 
 * @param nan_boxed NaN-boxed value
 * @return int 1 if function, 0 otherwise
 */
int nan_boxing_is_function(NanBoxedValue nan_boxed);

/**
 * @brief Get number value from NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return double Number value
 */
double nan_boxing_get_number(NanBoxedValue nan_boxed);

/**
 * @brief Get integer value from NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return int32_t Integer value
 */
int32_t nan_boxing_get_integer(NanBoxedValue nan_boxed);

/**
 * @brief Get boolean value from NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return int Boolean value (0 or 1)
 */
int nan_boxing_get_boolean(NanBoxedValue nan_boxed);

/**
 * @brief Get string pointer from NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return char* String pointer
 */
char* nan_boxing_get_string(NanBoxedValue nan_boxed);

/**
 * @brief Get array pointer from NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return Value** Array pointer
 */
Value** nan_boxing_get_array(NanBoxedValue nan_boxed);

/**
 * @brief Get object pointer from NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return void* Object pointer
 */
void* nan_boxing_get_object(NanBoxedValue nan_boxed);

/**
 * @brief Get function pointer from NaN-boxed value
 * 
 * @param nan_boxed NaN-boxed value
 * @return void* Function pointer
 */
void* nan_boxing_get_function(NanBoxedValue nan_boxed);

/**
 * @brief Create NaN-boxed number
 * 
 * @param number Number value
 * @return NanBoxedValue NaN-boxed number
 */
NanBoxedValue nan_boxing_create_number(double number);

/**
 * @brief Create NaN-boxed integer
 * 
 * @param integer Integer value
 * @return NanBoxedValue NaN-boxed integer
 */
NanBoxedValue nan_boxing_create_integer(int32_t integer);

/**
 * @brief Create NaN-boxed boolean
 * 
 * @param boolean Boolean value
 * @return NanBoxedValue NaN-boxed boolean
 */
NanBoxedValue nan_boxing_create_boolean(int boolean);

/**
 * @brief Create NaN-boxed null
 * 
 * @return NanBoxedValue NaN-boxed null
 */
NanBoxedValue nan_boxing_create_null(void);

/**
 * @brief Create NaN-boxed string
 * 
 * @param string String pointer
 * @return NanBoxedValue NaN-boxed string
 */
NanBoxedValue nan_boxing_create_string(char* string);

/**
 * @brief Create NaN-boxed array
 * 
 * @param array Array pointer
 * @return NanBoxedValue NaN-boxed array
 */
NanBoxedValue nan_boxing_create_array(Value** array);

/**
 * @brief Create NaN-boxed object
 * 
 * @param object Object pointer
 * @return NanBoxedValue NaN-boxed object
 */
NanBoxedValue nan_boxing_create_object(void* object);

/**
 * @brief Create NaN-boxed function
 * 
 * @param function Function pointer
 * @return NanBoxedValue NaN-boxed function
 */
NanBoxedValue nan_boxing_create_function(void* function);

/**
 * @brief Get NaN-boxing statistics
 * 
 * @param context NaN-boxing context
 * @param conversion_count Number of conversions performed
 * @param memory_saved Memory saved by NaN-boxing
 */
void nan_boxing_get_stats(NanBoxingContext* context,
                          size_t* conversion_count,
                          size_t* memory_saved);

#endif // MYCO_NAN_BOXING_H
