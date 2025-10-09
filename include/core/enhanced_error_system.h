#ifndef ENHANCED_ERROR_SYSTEM_H
#define ENHANCED_ERROR_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

// ============================================================================
// ERROR SEVERITY LEVELS
// ============================================================================

typedef enum {
    ERROR_SEVERITY_INFO = 0,
    ERROR_SEVERITY_WARNING = 1,
    ERROR_SEVERITY_ERROR = 2,
    ERROR_SEVERITY_CRITICAL = 3,
    ERROR_SEVERITY_FATAL = 4
} ErrorSeverity;

// ============================================================================
// ERROR CATEGORIES
// ============================================================================

typedef enum {
    ERROR_CATEGORY_LEXICAL = 0,
    ERROR_CATEGORY_SYNTAX = 1,
    ERROR_CATEGORY_SEMANTIC = 2,
    ERROR_CATEGORY_TYPE = 3,
    ERROR_CATEGORY_RUNTIME = 4,
    ERROR_CATEGORY_MEMORY = 5,
    ERROR_CATEGORY_IO = 6,
    ERROR_CATEGORY_NETWORK = 7,
    ERROR_CATEGORY_SYSTEM = 8,
    ERROR_CATEGORY_USER = 9
} ErrorCategory;

// ============================================================================
// ENHANCED ERROR CODES
// ============================================================================

typedef enum {
    // Lexical errors (1000-1999)
    MYCO_ERROR_UNKNOWN_TOKEN = 1000,
    MYCO_ERROR_UNTERMINATED_STRING = 1001,
    MYCO_ERROR_UNTERMINATED_COMMENT = 1002,
    MYCO_ERROR_INVALID_NUMBER = 1003,
    MYCO_ERROR_INVALID_ESCAPE = 1004,
    MYCO_ERROR_INVALID_CHARACTER = 1005,
    MYCO_ERROR_EOF_IN_STRING = 1006,
    MYCO_ERROR_EOF_IN_COMMENT = 1007,
    
    // Syntax errors (2000-2999)
    MYCO_ERROR_UNEXPECTED_TOKEN = 2000,
    MYCO_ERROR_MISSING_SEMICOLON = 2001,
    MYCO_ERROR_MISSING_PARENTHESIS = 2002,
    MYCO_ERROR_MISSING_BRACE = 2003,
    MYCO_ERROR_MISSING_BRACKET = 2004,
    MYCO_ERROR_INVALID_EXPRESSION = 2005,
    MYCO_ERROR_INVALID_STATEMENT = 2006,
    MYCO_ERROR_INVALID_FUNCTION = 2007,
    MYCO_ERROR_INVALID_CLASS = 2008,
    MYCO_ERROR_INVALID_IMPORT = 2009,
    MYCO_ERROR_INVALID_MATCH = 2010,
    MYCO_ERROR_INVALID_LOOP = 2011,
    MYCO_ERROR_INVALID_CONDITION = 2012,
    
    // Semantic errors (3000-3999)
    MYCO_ERROR_UNDEFINED_VARIABLE = 3000,
    MYCO_ERROR_UNDEFINED_FUNCTION = 3001,
    MYCO_ERROR_UNDEFINED_CLASS = 3002,
    MYCO_ERROR_TYPE_MISMATCH = 3003,
    MYCO_ERROR_ARGUMENT_COUNT = 3004,
    MYCO_ERROR_INVALID_ARGUMENT = 3005,
    MYCO_ERROR_DUPLICATE_DECLARATION = 3006,
    MYCO_ERROR_INVALID_OPERATION = 3007,
    MYCO_ERROR_INVALID_CAST = 3008,
    MYCO_ERROR_INVALID_ACCESS = 3009,
    MYCO_ERROR_SCOPE_VIOLATION = 3010,
    MYCO_ERROR_CONSTANT_REASSIGNMENT = 3011,
    MYCO_ERROR_READONLY_MODIFICATION = 3012,
    
    // Runtime errors (4000-4999)
    MYCO_ERROR_DIVISION_BY_ZERO = 4000,
    MYCO_ERROR_ARRAY_BOUNDS = 4001,
    MYCO_ERROR_NULL_POINTER = 4002,
    MYCO_ERROR_STACK_OVERFLOW = 4003,
    MYCO_ERROR_STACK_UNDERFLOW = 4004,
    MYCO_ERROR_INVALID_INDEX = 4005,
    MYCO_ERROR_INVALID_KEY = 4006,
    MYCO_ERROR_INVALID_RUNTIME_OPERATION = 4007,
    MYCO_ERROR_INVALID_STATE = 4008,
    MYCO_ERROR_INVALID_CONTEXT = 4009,
    MYCO_ERROR_RECURSION_LIMIT = 4010,
    MYCO_ERROR_TIMEOUT = 4011,
    MYCO_ERROR_ASSERTION_FAILED = 4012,
    
    // Memory errors (5000-5999)
    MYCO_ERROR_OUT_OF_MEMORY = 5000,
    MYCO_ERROR_MEMORY_CORRUPTION = 5001,
    MYCO_ERROR_DOUBLE_FREE = 5002,
    MYCO_ERROR_MEMORY_LEAK = 5003,
    MYCO_ERROR_INVALID_POINTER = 5004,
    MYCO_ERROR_BUFFER_OVERFLOW = 5005,
    MYCO_ERROR_BUFFER_UNDERFLOW = 5006,
    MYCO_ERROR_INVALID_SIZE = 5007,
    MYCO_ERROR_INVALID_ALIGNMENT = 5008,
    MYCO_ERROR_MEMORY_EXHAUSTED = 5009,
    MYCO_ERROR_USE_AFTER_FREE = 5010,
    MYCO_ERROR_WILD_POINTER = 5011,
    
    // I/O errors (6000-6999)
    MYCO_ERROR_FILE_NOT_FOUND = 6000,
    MYCO_ERROR_FILE_ACCESS = 6001,
    MYCO_ERROR_FILE_PERMISSION = 6002,
    MYCO_ERROR_FILE_CORRUPTION = 6003,
    MYCO_ERROR_DISK_FULL = 6004,
    MYCO_ERROR_READ_ERROR = 6005,
    MYCO_ERROR_WRITE_ERROR = 6006,
    MYCO_ERROR_SEEK_ERROR = 6007,
    MYCO_ERROR_FLUSH_ERROR = 6008,
    
    // Network errors (7000-7999)
    MYCO_ERROR_NETWORK_ERROR = 7000,
    MYCO_ERROR_CONNECTION_FAILED = 7001,
    MYCO_ERROR_CONNECTION_TIMEOUT = 7002,
    MYCO_ERROR_CONNECTION_REFUSED = 7003,
    MYCO_ERROR_DNS_FAILURE = 7004,
    MYCO_ERROR_SSL_ERROR = 7005,
    MYCO_ERROR_PROTOCOL_ERROR = 7006,
    
    // System errors (8000-8999)
    MYCO_ERROR_SYSTEM_ERROR = 8000,
    MYCO_ERROR_PROCESS_FAILED = 8001,
    MYCO_ERROR_SIGNAL_RECEIVED = 8002,
    MYCO_ERROR_RESOURCE_EXHAUSTED = 8003,
    MYCO_ERROR_PLATFORM_ERROR = 8004,
    MYCO_ERROR_EXTERNAL_ERROR = 8005,
    MYCO_ERROR_CONFIGURATION_ERROR = 8006,
    MYCO_ERROR_DEPENDENCY_MISSING = 8007,
    
    // Compilation errors (9000-9999)
    MYCO_ERROR_COMPILATION_FAILED = 9000,
    MYCO_ERROR_OPTIMIZATION_FAILED = 9001,
    MYCO_ERROR_CODE_GENERATION_FAILED = 9002,
    MYCO_ERROR_LINKING_FAILED = 9003,
    MYCO_ERROR_ASSEMBLY_FAILED = 9004,
    MYCO_ERROR_TARGET_NOT_SUPPORTED = 9005,
    MYCO_ERROR_INVALID_TARGET = 9006,
    MYCO_ERROR_COMPILER_BUG = 9007,
    MYCO_ERROR_INTERNAL_ERROR = 9008,
    MYCO_ERROR_UNIMPLEMENTED = 9009,
    
    // Advanced edge case errors (10000-10099)
    MYCO_ERROR_INFINITE_LOOP = 10000,
    MYCO_ERROR_DEADLOCK = 10001,
    MYCO_ERROR_RACE_CONDITION = 10002,
    MYCO_ERROR_MEMORY_FRAGMENTATION = 10003,
    MYCO_ERROR_ALIGNMENT_FAULT = 10004,
    MYCO_ERROR_ACCESS_VIOLATION = 10005,
    MYCO_ERROR_STACK_SMASHING = 10006,
    MYCO_ERROR_HEAP_CORRUPTION = 10007,
    MYCO_ERROR_DANGLING_POINTER = 10008,
    MYCO_ERROR_MEMORY_ALIASING = 10009,
    MYCO_ERROR_ENDIAN_MISMATCH = 10010,
    MYCO_ERROR_OVERFLOW = 10011,
    MYCO_ERROR_UNDERFLOW = 10012,
    MYCO_ERROR_PRECISION_LOSS = 10013,
    MYCO_ERROR_NUMERICAL_INSTABILITY = 10014,
    MYCO_ERROR_CONVERGENCE_FAILURE = 10015,
    MYCO_ERROR_ITERATION_LIMIT = 10016,
    MYCO_ERROR_DIVISION_BY_NEAR_ZERO = 10017,
    MYCO_ERROR_SQRT_NEGATIVE = 10018,
    MYCO_ERROR_LOG_NEGATIVE = 10019,
    MYCO_ERROR_ACOS_OUT_OF_RANGE = 10020,
    MYCO_ERROR_ASIN_OUT_OF_RANGE = 10021,
    MYCO_ERROR_TAN_INFINITY = 10022,
    MYCO_ERROR_POW_INFINITY = 10023,
    MYCO_ERROR_EXP_OVERFLOW = 10024,
    MYCO_ERROR_LOG_UNDERFLOW = 10025,
    MYCO_ERROR_SINH_OVERFLOW = 10026,
    MYCO_ERROR_COSH_OVERFLOW = 10027,
    MYCO_ERROR_TANH_UNDERFLOW = 10028,
    MYCO_ERROR_ATAN2_UNDEFINED = 10029,
    MYCO_ERROR_HYPOT_OVERFLOW = 10030,
    MYCO_ERROR_FMOD_DIVISION_BY_ZERO = 10031,
    MYCO_ERROR_REMAINDER_DIVISION_BY_ZERO = 10032,
    MYCO_ERROR_SCALBN_OVERFLOW = 10033,
    MYCO_ERROR_FREXP_INVALID = 10034,
    MYCO_ERROR_LDEXP_OVERFLOW = 10035,
    MYCO_ERROR_MODF_INVALID = 10036,
    MYCO_ERROR_FRAC_INVALID = 10037,
    MYCO_ERROR_TRUNC_INVALID = 10038,
    MYCO_ERROR_ROUND_INVALID = 10039,
    MYCO_ERROR_NEARBYINT_INVALID = 10040,
    MYCO_ERROR_RINT_INVALID = 10041,
    MYCO_ERROR_LRINT_INVALID = 10042,
    MYCO_ERROR_LLRINT_INVALID = 10043,
    MYCO_ERROR_LROUND_INVALID = 10044,
    MYCO_ERROR_LLROUND_INVALID = 10045,
    MYCO_ERROR_ILOGB_ZERO = 10046,
    MYCO_ERROR_ILOGB_INFINITY = 10047,
    MYCO_ERROR_ILOGB_NAN = 10048,
    MYCO_ERROR_LOG1P_OVERFLOW = 10049,
    MYCO_ERROR_LOG1P_UNDERFLOW = 10050,
    MYCO_ERROR_EXPM1_OVERFLOW = 10051,
    MYCO_ERROR_EXPM1_UNDERFLOW = 10052,
    MYCO_ERROR_POW10_OVERFLOW = 10053,
    MYCO_ERROR_POW10_UNDERFLOW = 10054,
    MYCO_ERROR_EXP2_OVERFLOW = 10055,
    MYCO_ERROR_EXP2_UNDERFLOW = 10056,
    MYCO_ERROR_LOG2_OVERFLOW = 10057,
    MYCO_ERROR_LOG2_UNDERFLOW = 10058,
    MYCO_ERROR_SQRTM1_INVALID = 10059,
    MYCO_ERROR_ACOSH_OUT_OF_RANGE = 10060,
    MYCO_ERROR_ASINH_OVERFLOW = 10061,
    MYCO_ERROR_ATANH_OUT_OF_RANGE = 10062,
    MYCO_ERROR_CBRT_OVERFLOW = 10063,
    MYCO_ERROR_FMA_OVERFLOW = 10064,
    MYCO_ERROR_FMA_UNDERFLOW = 10065,
    MYCO_ERROR_FMAX_INVALID = 10066,
    MYCO_ERROR_FMIN_INVALID = 10067,
    MYCO_ERROR_FDIM_INVALID = 10068,
    MYCO_ERROR_FABS_INVALID = 10069,
    MYCO_ERROR_COPYSIGN_INVALID = 10070,
    MYCO_ERROR_NEXTAFTER_OVERFLOW = 10071,
    MYCO_ERROR_NEXTAFTER_UNDERFLOW = 10072,
    MYCO_ERROR_NEXTTOWARD_OVERFLOW = 10073,
    MYCO_ERROR_NEXTTOWARD_UNDERFLOW = 10074,
    MYCO_ERROR_REMAINDER_INVALID = 10075,
    MYCO_ERROR_REMAINDER_INFINITY = 10076,
    MYCO_ERROR_REMAINDER_NAN = 10077,
    MYCO_ERROR_NAN_INVALID = 10078,
    MYCO_ERROR_INFINITY_INVALID = 10079,
    MYCO_ERROR_FINITE_INVALID = 10080,
    MYCO_ERROR_ISNAN_INVALID = 10081,
    MYCO_ERROR_ISINF_INVALID = 10082,
    MYCO_ERROR_ISFINITE_INVALID = 10083,
    MYCO_ERROR_ISNORMAL_INVALID = 10084,
    MYCO_ERROR_SIGNBIT_INVALID = 10085,
    MYCO_ERROR_ISGREATER_INVALID = 10086,
    MYCO_ERROR_ISGREATEREQUAL_INVALID = 10087,
    MYCO_ERROR_ISLESS_INVALID = 10088,
    MYCO_ERROR_ISLESSEQUAL_INVALID = 10089,
    MYCO_ERROR_ISLESSGREATER_INVALID = 10090,
    MYCO_ERROR_ISUNORDERED_INVALID = 10091,
    MYCO_ERROR_FPCLASSIFY_INVALID = 10092,
    MYCO_ERROR_ISUNORDERED_INFINITY = 10093,
    MYCO_ERROR_ISUNORDERED_NAN = 10094,
    MYCO_ERROR_ISUNORDERED_ZERO = 10095,
    MYCO_ERROR_ISUNORDERED_SUBNORMAL = 10096,
    MYCO_ERROR_ISUNORDERED_NORMAL = 10097,
    MYCO_ERROR_ISUNORDERED_NEGATIVE = 10098,
    
    // String and text processing edge cases (10100-10199)
    MYCO_ERROR_STRING_OVERFLOW = 10100,
    MYCO_ERROR_STRING_UNDERFLOW = 10101,
    MYCO_ERROR_STRING_NULL_TERMINATOR = 10102,
    MYCO_ERROR_STRING_ENCODING = 10103,
    MYCO_ERROR_STRING_DECODING = 10104,
    MYCO_ERROR_STRING_TRUNCATION = 10105,
    MYCO_ERROR_STRING_PADDING = 10106,
    MYCO_ERROR_STRING_ESCAPE = 10107,
    MYCO_ERROR_STRING_UNICODE = 10108,
    MYCO_ERROR_STRING_UTF8 = 10109,
    MYCO_ERROR_STRING_UTF16 = 10110,
    MYCO_ERROR_STRING_UTF32 = 10111,
    MYCO_ERROR_STRING_ASCII = 10112,
    MYCO_ERROR_STRING_LATIN1 = 10113,
    MYCO_ERROR_STRING_ISO8859 = 10114,
    MYCO_ERROR_STRING_WINDOWS1252 = 10115,
    MYCO_ERROR_STRING_CP437 = 10116,
    MYCO_ERROR_STRING_CP850 = 10117,
    MYCO_ERROR_STRING_CP1252 = 10118,
    MYCO_ERROR_STRING_KOI8 = 10119,
    MYCO_ERROR_STRING_EBCDIC = 10120,
    MYCO_ERROR_STRING_BIG5 = 10121,
    MYCO_ERROR_STRING_GB2312 = 10122,
    MYCO_ERROR_STRING_GBK = 10123,
    MYCO_ERROR_STRING_GB18030 = 10124,
    MYCO_ERROR_STRING_SHIFT_JIS = 10125,
    MYCO_ERROR_STRING_EUC_JP = 10126,
    MYCO_ERROR_STRING_EUC_KR = 10127,
    MYCO_ERROR_STRING_ISO2022 = 10128,
    MYCO_ERROR_STRING_HZ = 10129,
    MYCO_ERROR_STRING_BOCU1 = 10130,
    MYCO_ERROR_STRING_SCSU = 10131,
    MYCO_ERROR_STRING_UTF7 = 10132,
    MYCO_ERROR_STRING_UTF1 = 10133,
    MYCO_ERROR_STRING_UTF_EB = 10134,
    MYCO_ERROR_STRING_UTF_EB_CD = 10135,
    MYCO_ERROR_STRING_UTF_EB_CD_OC = 10136,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC = 10137,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC = 10138,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC = 10139,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC = 10140,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC = 10141,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC = 10142,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC = 10143,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10144,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10145,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10146,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10147,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10148,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10149,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10150,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10151,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10152,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10153,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10154,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10155,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10156,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10157,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10158,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10159,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10160,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10161,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10162,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10163,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10164,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10165,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10166,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10167,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10168,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10169,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10170,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10171,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10172,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10173,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10174,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10175,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10176,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10177,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10178,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10179,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10180,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10181,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10182,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10183,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10184,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10185,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10186,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10187,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10188,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10189,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10190,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10191,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10192,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10193,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10194,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10195,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10196,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC = 10197,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC = 10198,
    MYCO_ERROR_STRING_UTF_EB_CD_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC_IC_UC_OC = 10199,
    
    // Concurrency and threading edge cases (10200-10299)
    MYCO_ERROR_THREAD_CREATION_FAILED = 10200,
    MYCO_ERROR_THREAD_JOIN_FAILED = 10201,
    MYCO_ERROR_THREAD_DETACH_FAILED = 10202,
    MYCO_ERROR_THREAD_CANCEL_FAILED = 10203,
    MYCO_ERROR_THREAD_ALREADY_JOINED = 10204,
    MYCO_ERROR_THREAD_ALREADY_DETACHED = 10205,
    MYCO_ERROR_THREAD_NOT_JOINABLE = 10206,
    MYCO_ERROR_THREAD_NOT_DETACHED = 10207,
    MYCO_ERROR_THREAD_DEADLOCK = 10208,
    MYCO_ERROR_THREAD_LIVELOCK = 10209,
    MYCO_ERROR_THREAD_STARVATION = 10210,
    MYCO_ERROR_THREAD_PRIORITY = 10211,
    MYCO_ERROR_THREAD_AFFINITY = 10212,
    MYCO_ERROR_THREAD_STACK_SIZE = 10213,
    MYCO_ERROR_THREAD_STACK_OVERFLOW = 10214,
    MYCO_ERROR_THREAD_STACK_UNDERFLOW = 10215,
    MYCO_ERROR_THREAD_STACK_CORRUPTION = 10216,
    MYCO_ERROR_THREAD_STACK_ALIGNMENT = 10217,
    MYCO_ERROR_THREAD_STACK_PROTECTION = 10218,
    MYCO_ERROR_THREAD_STACK_GUARD = 10219,
    MYCO_ERROR_THREAD_STACK_RED_ZONE = 10220,
    MYCO_ERROR_THREAD_STACK_YELLOW_ZONE = 10221,
    MYCO_ERROR_THREAD_STACK_GREEN_ZONE = 10222,
    MYCO_ERROR_THREAD_STACK_BLUE_ZONE = 10223,
    MYCO_ERROR_THREAD_STACK_PURPLE_ZONE = 10224,
    MYCO_ERROR_THREAD_STACK_ORANGE_ZONE = 10225,
    MYCO_ERROR_THREAD_STACK_PINK_ZONE = 10226,
    MYCO_ERROR_THREAD_STACK_CYAN_ZONE = 10227,
    MYCO_ERROR_THREAD_STACK_MAGENTA_ZONE = 10228,
    MYCO_ERROR_THREAD_STACK_LIME_ZONE = 10229,
    MYCO_ERROR_THREAD_STACK_INDIGO_ZONE = 10230,
    MYCO_ERROR_THREAD_STACK_VIOLET_ZONE = 10231,
    MYCO_ERROR_THREAD_STACK_TEAL_ZONE = 10232,
    MYCO_ERROR_THREAD_STACK_NAVY_ZONE = 10233,
    MYCO_ERROR_THREAD_STACK_MAROON_ZONE = 10234,
    MYCO_ERROR_THREAD_STACK_OLIVE_ZONE = 10235,
    MYCO_ERROR_THREAD_STACK_AQUA_ZONE = 10236,
    MYCO_ERROR_THREAD_STACK_SILVER_ZONE = 10237,
    MYCO_ERROR_THREAD_STACK_GRAY_ZONE = 10238,
    MYCO_ERROR_THREAD_STACK_BLACK_ZONE = 10239,
    MYCO_ERROR_THREAD_STACK_WHITE_ZONE = 10240,
    MYCO_ERROR_THREAD_STACK_TRANSPARENT_ZONE = 10241,
    MYCO_ERROR_THREAD_STACK_OPAQUE_ZONE = 10242,
    MYCO_ERROR_THREAD_STACK_SEMI_TRANSPARENT_ZONE = 10243,
    MYCO_ERROR_THREAD_STACK_ALPHA_ZONE = 10244,
    MYCO_ERROR_THREAD_STACK_BETA_ZONE = 10245,
    MYCO_ERROR_THREAD_STACK_GAMMA_ZONE = 10246,
    MYCO_ERROR_THREAD_STACK_DELTA_ZONE = 10247,
    MYCO_ERROR_THREAD_STACK_EPSILON_ZONE = 10248,
    MYCO_ERROR_THREAD_STACK_ZETA_ZONE = 10249,
    MYCO_ERROR_THREAD_STACK_ETA_ZONE = 10250,
    MYCO_ERROR_THREAD_STACK_THETA_ZONE = 10251,
    MYCO_ERROR_THREAD_STACK_IOTA_ZONE = 10252,
    MYCO_ERROR_THREAD_STACK_KAPPA_ZONE = 10253,
    MYCO_ERROR_THREAD_STACK_LAMBDA_ZONE = 10254,
    MYCO_ERROR_THREAD_STACK_MU_ZONE = 10255,
    MYCO_ERROR_THREAD_STACK_NU_ZONE = 10256,
    MYCO_ERROR_THREAD_STACK_XI_ZONE = 10257,
    MYCO_ERROR_THREAD_STACK_OMICRON_ZONE = 10258,
    MYCO_ERROR_THREAD_STACK_PI_ZONE = 10259,
    MYCO_ERROR_THREAD_STACK_RHO_ZONE = 10260,
    MYCO_ERROR_THREAD_STACK_SIGMA_ZONE = 10261,
    MYCO_ERROR_THREAD_STACK_TAU_ZONE = 10262,
    MYCO_ERROR_THREAD_STACK_UPSILON_ZONE = 10263,
    MYCO_ERROR_THREAD_STACK_PHI_ZONE = 10264,
    MYCO_ERROR_THREAD_STACK_CHI_ZONE = 10265,
    MYCO_ERROR_THREAD_STACK_PSI_ZONE = 10266,
    MYCO_ERROR_THREAD_STACK_OMEGA_ZONE = 10267,
    MYCO_ERROR_THREAD_STACK_ALPHA_BETA_ZONE = 10268,
    MYCO_ERROR_THREAD_STACK_GAMMA_DELTA_ZONE = 10269,
    MYCO_ERROR_THREAD_STACK_EPSILON_ZETA_ZONE = 10270,
    MYCO_ERROR_THREAD_STACK_ETA_THETA_ZONE = 10271,
    MYCO_ERROR_THREAD_STACK_IOTA_KAPPA_ZONE = 10272,
    MYCO_ERROR_THREAD_STACK_LAMBDA_MU_ZONE = 10273,
    MYCO_ERROR_THREAD_STACK_NU_XI_ZONE = 10274,
    MYCO_ERROR_THREAD_STACK_OMICRON_PI_ZONE = 10275,
    MYCO_ERROR_THREAD_STACK_RHO_SIGMA_ZONE = 10276,
    MYCO_ERROR_THREAD_STACK_TAU_UPSILON_ZONE = 10277,
    MYCO_ERROR_THREAD_STACK_PHI_CHI_ZONE = 10278,
    MYCO_ERROR_THREAD_STACK_PSI_OMEGA_ZONE = 10279,
    MYCO_ERROR_THREAD_STACK_ALPHA_GAMMA_ZONE = 10280,
    MYCO_ERROR_THREAD_STACK_BETA_DELTA_ZONE = 10281,
    MYCO_ERROR_THREAD_STACK_EPSILON_ETA_ZONE = 10282,
    MYCO_ERROR_THREAD_STACK_ZETA_THETA_ZONE = 10283,
    MYCO_ERROR_THREAD_STACK_IOTA_LAMBDA_ZONE = 10284,
    MYCO_ERROR_THREAD_STACK_KAPPA_MU_ZONE = 10285,
    MYCO_ERROR_THREAD_STACK_NU_OMICRON_ZONE = 10286,
    MYCO_ERROR_THREAD_STACK_XI_PI_ZONE = 10287,
    MYCO_ERROR_THREAD_STACK_RHO_TAU_ZONE = 10288,
    MYCO_ERROR_THREAD_STACK_SIGMA_UPSILON_ZONE = 10289,
    MYCO_ERROR_THREAD_STACK_PHI_PSI_ZONE = 10290,
    MYCO_ERROR_THREAD_STACK_CHI_OMEGA_ZONE = 10291,
    MYCO_ERROR_THREAD_STACK_ALPHA_DELTA_ZONE = 10292,
    MYCO_ERROR_THREAD_STACK_BETA_GAMMA_ZONE = 10293,
    MYCO_ERROR_THREAD_STACK_EPSILON_THETA_ZONE = 10294,
    MYCO_ERROR_THREAD_STACK_ZETA_ETA_ZONE = 10295,
    MYCO_ERROR_THREAD_STACK_IOTA_MU_ZONE = 10296,
    MYCO_ERROR_THREAD_STACK_KAPPA_LAMBDA_ZONE = 10297,
    MYCO_ERROR_THREAD_STACK_NU_PI_ZONE = 10298,
    MYCO_ERROR_THREAD_STACK_XI_OMICRON_ZONE = 10299,
    
    // User-defined errors (11000+)
    MYCO_ERROR_USER_DEFINED = 11000
} MycoErrorCode;

// ============================================================================
// ERROR RECOVERY MECHANISMS
// ============================================================================

typedef enum {
    RECOVERY_STRATEGY_NONE = 0,
    RECOVERY_STRATEGY_RETRY = 1,
    RECOVERY_STRATEGY_FALLBACK = 2,
    RECOVERY_STRATEGY_IGNORE = 3,
    RECOVERY_STRATEGY_ABORT = 4,
    RECOVERY_STRATEGY_RECOVER = 5,
    RECOVERY_STRATEGY_CONTINUE = 6
} ErrorRecoveryStrategy;

typedef struct {
    MycoErrorCode error_code;
    ErrorRecoveryStrategy strategy;
    int max_retries;
    int retry_delay_ms;
    char* fallback_message;
    void* recovery_data;
    bool (*recovery_function)(void* context, void* data);
} ErrorRecoveryRule;

// ============================================================================
// ENHANCED ERROR CONTEXT
// ============================================================================

typedef struct {
    char* variable_name;
    char* variable_type;
    char* variable_value;
    char* scope_info;
    char* memory_address;
    char* last_assignment;
} VariableContext;

typedef struct {
    char* function_name;
    char* function_signature;
    char* parameter_types;
    char* return_type;
    char* call_stack;
    int recursion_depth;
} FunctionContext;

typedef struct {
    char* file_path;
    char* file_content;
    char* file_encoding;
    size_t file_size;
    time_t file_modified;
    char* file_permissions;
} FileContext;

typedef struct {
    VariableContext* variables;
    FunctionContext* functions;
    FileContext* files;
    size_t variable_count;
    size_t function_count;
    size_t file_count;
    char* system_info;
    char* memory_info;
    char* performance_info;
} EnhancedErrorContext;


// ============================================================================
// STACK FRAME STRUCTURE
// ============================================================================

typedef struct {
    char* function_name;
    char* file_name;
    uint32_t line_number;
    uint32_t column_number;
    char* source_line;
    char* context_info;
} StackFrame;

// ============================================================================
// ENHANCED ERROR INFO STRUCTURE
// ============================================================================

typedef struct EnhancedErrorInfo {
    MycoErrorCode code;
    ErrorSeverity severity;
    ErrorCategory category;
    char* message;
    char* file_name;
    uint32_t line_number;
    uint32_t column_number;
    char* source_line;
    char* suggestion;
    char* context;
    char* variable_context;
    void* user_data;
    
    // Stack trace information
    StackFrame* stack_trace;
    size_t stack_trace_size;
    size_t stack_trace_capacity;
    
    // Timing information
    uint64_t timestamp;
    uint64_t execution_time;
    
    // Related errors
    struct EnhancedErrorInfo* related_errors;
    size_t related_count;
    
    // Error chain (for nested errors)
    struct EnhancedErrorInfo* cause;
    struct EnhancedErrorInfo* next;
} EnhancedErrorInfo;

// ============================================================================
// ERROR SYSTEM CONFIGURATION
// ============================================================================

typedef struct {
    bool colors_enabled;
    bool verbose_mode;
    bool stack_trace_enabled;
    bool timing_enabled;
    bool context_enabled;
    bool suggestions_enabled;
    bool auto_recovery_enabled;
    
    size_t max_stack_depth;
    size_t max_error_count;
    size_t max_context_length;
    
    char* log_file;
    char* component_filter;
    
    // Error handlers
    void (*error_handler)(const EnhancedErrorInfo* error);
    void (*warning_handler)(const EnhancedErrorInfo* error);
    void (*critical_handler)(const EnhancedErrorInfo* error);
} ErrorSystemConfig;

// ============================================================================
// ENHANCED ERROR SYSTEM
// ============================================================================

typedef struct {
    EnhancedErrorInfo** errors;
    size_t error_count;
    size_t error_capacity;
    
    ErrorSystemConfig config;
    
    // Error statistics
    uint64_t total_errors;
    uint64_t error_counts[5]; // By severity
    uint64_t category_counts[10]; // By category
    
    // Performance tracking
    uint64_t start_time;
    uint64_t last_error_time;
    
    // Recovery state
    bool in_recovery_mode;
    size_t recovery_attempts;
    
    // Recovery rules
    ErrorRecoveryRule* recovery_rules;
    size_t recovery_rules_count;
    size_t recovery_rules_capacity;
} EnhancedErrorSystem;

// ============================================================================
// ERROR RECOVERY FUNCTIONS
// ============================================================================

/**
 * @brief Add a recovery rule for a specific error code
 * @param system The error system instance
 * @param rule The recovery rule to add
 * @return true if successful, false otherwise
 */
bool enhanced_error_add_recovery_rule(EnhancedErrorSystem* system, const ErrorRecoveryRule* rule);

/**
 * @brief Remove a recovery rule for a specific error code
 * @param system The error system instance
 * @param error_code The error code to remove recovery rule for
 * @return true if successful, false otherwise
 */
bool enhanced_error_remove_recovery_rule(EnhancedErrorSystem* system, MycoErrorCode error_code);

/**
 * @brief Get recovery rule for a specific error code
 * @param system The error system instance
 * @param error_code The error code to get recovery rule for
 * @return Pointer to recovery rule or NULL if not found
 */
const ErrorRecoveryRule* enhanced_error_get_recovery_rule(EnhancedErrorSystem* system, MycoErrorCode error_code);

/**
 * @brief Attempt to recover from an error using the configured recovery strategy
 * @param system The error system instance
 * @param error The error to recover from
 * @param context The context for recovery
 * @return true if recovery was successful, false otherwise
 */
bool enhanced_error_attempt_recovery(EnhancedErrorSystem* system, const EnhancedErrorInfo* error, void* context);

// ============================================================================
// ENHANCED ERROR CONTEXT FUNCTIONS
// ============================================================================

/**
 * @brief Create enhanced error context
 * @return Pointer to new context or NULL on failure
 */
EnhancedErrorContext* enhanced_error_context_create(void);

/**
 * @brief Free enhanced error context
 * @param context The context to free
 */
void enhanced_error_context_free(EnhancedErrorContext* context);

/**
 * @brief Add variable context to error context
 * @param context The error context
 * @param var_name Variable name
 * @param var_type Variable type
 * @param var_value Variable value
 * @param scope_info Scope information
 * @return true if successful, false otherwise
 */
bool enhanced_error_context_add_variable(EnhancedErrorContext* context, const char* var_name, 
                                        const char* var_type, const char* var_value, const char* scope_info);

/**
 * @brief Add function context to error context
 * @param context The error context
 * @param func_name Function name
 * @param func_signature Function signature
 * @param param_types Parameter types
 * @param return_type Return type
 * @param call_stack Call stack information
 * @return true if successful, false otherwise
 */
bool enhanced_error_context_add_function(EnhancedErrorContext* context, const char* func_name,
                                        const char* func_signature, const char* param_types,
                                        const char* return_type, const char* call_stack);

/**
 * @brief Add file context to error context
 * @param context The error context
 * @param file_path File path
 * @param file_content File content
 * @param file_encoding File encoding
 * @return true if successful, false otherwise
 */
bool enhanced_error_context_add_file(EnhancedErrorContext* context, const char* file_path,
                                    const char* file_content, const char* file_encoding);

/**
 * @brief Set system information in error context
 * @param context The error context
 * @param system_info System information string
 * @return true if successful, false otherwise
 */
bool enhanced_error_context_set_system_info(EnhancedErrorContext* context, const char* system_info);

/**
 * @brief Set memory information in error context
 * @param context The error context
 * @param memory_info Memory information string
 * @return true if successful, false otherwise
 */
bool enhanced_error_context_set_memory_info(EnhancedErrorContext* context, const char* memory_info);

/**
 * @brief Set performance information in error context
 * @param context The error context
 * @param performance_info Performance information string
 * @return true if successful, false otherwise
 */
bool enhanced_error_context_set_performance_info(EnhancedErrorContext* context, const char* performance_info);

// ============================================================================
// ENHANCED ERROR REPORTING WITH CONTEXT
// ============================================================================

/**
 * @brief Report error with enhanced context
 * @param system The error system instance
 * @param error_code The error code
 * @param message The error message
 * @param file_name The file name
 * @param line The line number
 * @param column The column number
 * @param context The enhanced error context
 * @return true if successful, false otherwise
 */
bool enhanced_error_report_with_context(EnhancedErrorSystem* system, MycoErrorCode error_code,
                                       const char* message, const char* file_name,
                                       uint32_t line, uint32_t column, const EnhancedErrorContext* context);

/**
 * @brief Get detailed error analysis
 * @param error The error to analyze
 * @param context The error context
 * @return Detailed analysis string (must be freed by caller)
 */
char* enhanced_error_get_detailed_analysis(const EnhancedErrorInfo* error, const EnhancedErrorContext* context);

/**
 * @brief Get error suggestions based on context
 * @param error The error to get suggestions for
 * @param context The error context
 * @return Suggestions string (must be freed by caller)
 */
char* enhanced_error_get_contextual_suggestions(const EnhancedErrorInfo* error, const EnhancedErrorContext* context);

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// System initialization and cleanup
EnhancedErrorSystem* enhanced_error_system_create(void);
void enhanced_error_system_free(EnhancedErrorSystem* system);
void enhanced_error_system_configure(EnhancedErrorSystem* system, const ErrorSystemConfig* config);

// Error creation and management
EnhancedErrorInfo* enhanced_error_create(MycoErrorCode code, ErrorSeverity severity, 
                                        ErrorCategory category, const char* message,
                                        const char* file_name, uint32_t line, uint32_t column);
void enhanced_error_free(EnhancedErrorInfo* error);

// Error reporting
void enhanced_error_report(EnhancedErrorSystem* system, EnhancedErrorInfo* error);
void enhanced_error_report_simple(EnhancedErrorSystem* system, MycoErrorCode code, 
                                 const char* message, const char* file_name, 
                                 uint32_t line, uint32_t column);

// Error formatting and display
void enhanced_error_print(const EnhancedErrorInfo* error);
void enhanced_error_print_compact(const EnhancedErrorInfo* error);
void enhanced_error_print_verbose(const EnhancedErrorInfo* error);
char* enhanced_error_format(const EnhancedErrorInfo* error);
char* enhanced_error_format_json(const EnhancedErrorInfo* error);

// Stack trace management
void enhanced_error_add_stack_frame(EnhancedErrorInfo* error, const char* function_name,
                                   const char* file_name, uint32_t line, uint32_t column,
                                   const char* source_line, const char* context_info);
void enhanced_error_print_stack_trace(const EnhancedErrorInfo* error);

// Context and suggestions
void enhanced_error_add_context(EnhancedErrorInfo* error, const char* context);
void enhanced_error_add_suggestion(EnhancedErrorInfo* error, const char* suggestion);
void enhanced_error_add_variable_context(EnhancedErrorInfo* error, const char* variable_context);
void enhanced_error_add_source_line(EnhancedErrorInfo* error, const char* source_line);

// Error information retrieval
const char* enhanced_error_get_message(MycoErrorCode code);
const char* enhanced_error_get_suggestion(MycoErrorCode code);
ErrorSeverity enhanced_error_get_severity(MycoErrorCode code);
ErrorCategory enhanced_error_get_category(MycoErrorCode code);
const char* enhanced_error_get_category_name(ErrorCategory category);
const char* enhanced_error_get_severity_name(ErrorSeverity severity);

// Error system utilities
void enhanced_error_system_reset(EnhancedErrorSystem* system);
void enhanced_error_system_clear_errors(EnhancedErrorSystem* system);
size_t enhanced_error_system_get_error_count(EnhancedErrorSystem* system);
EnhancedErrorInfo* enhanced_error_system_get_last_error(EnhancedErrorSystem* system);
EnhancedErrorInfo* enhanced_error_system_get_error(EnhancedErrorSystem* system, size_t index);

// Error filtering and searching
EnhancedErrorInfo** enhanced_error_system_filter_by_severity(EnhancedErrorSystem* system, 
                                                           ErrorSeverity severity, size_t* count);
EnhancedErrorInfo** enhanced_error_system_filter_by_category(EnhancedErrorSystem* system, 
                                                           ErrorCategory category, size_t* count);
EnhancedErrorInfo** enhanced_error_system_filter_by_file(EnhancedErrorSystem* system, 
                                                        const char* file_name, size_t* count);

// Debugging and analysis
void enhanced_error_system_print_statistics(EnhancedErrorSystem* system);
void enhanced_error_system_print_summary(EnhancedErrorSystem* system);
void enhanced_error_system_export_log(EnhancedErrorSystem* system, const char* filename);

// Safe operations with error handling
void* enhanced_safe_malloc(size_t size, EnhancedErrorSystem* system, const char* context);
char* enhanced_safe_strdup(const char* str, EnhancedErrorSystem* system, const char* context);
FILE* enhanced_safe_fopen(const char* filename, const char* mode, EnhancedErrorSystem* system);
int enhanced_safe_divide(int a, int b, EnhancedErrorSystem* system, const char* context);

// Convenience macros
#define ENHANCED_ERROR_REPORT(system, code, message, file, line, col) \
    enhanced_error_report_simple(system, code, message, file, line, col)

#define ENHANCED_ERROR_REPORT_WITH_CONTEXT(system, code, message, file, line, col, context) \
    do { \
        EnhancedErrorInfo* error = enhanced_error_create(code, enhanced_error_get_severity(code), \
                                                        enhanced_error_get_category(code), message, \
                                                        file, line, col); \
        if (error) { \
            enhanced_error_add_context(error, context); \
            enhanced_error_report(system, error); \
        } \
    } while(0)

#define ENHANCED_ERROR_ASSERT(system, condition, code, message, file, line) \
    do { \
        if (!(condition)) { \
            ENHANCED_ERROR_REPORT(system, code, message, file, line, 0); \
        } \
    } while(0)

// ============================================================================
// GLOBAL SYSTEM MANAGEMENT
// ============================================================================

/**
 * @brief Get the global error system instance
 * @return Pointer to the global error system, or NULL if not initialized
 */
EnhancedErrorSystem* enhanced_error_system_get_global(void);

/**
 * @brief Initialize the global error system
 */
void enhanced_error_system_initialize_global(void);

/**
 * @brief Cleanup the global error system
 */
void enhanced_error_system_cleanup_global(void);

// ============================================================================
// COMPATIBILITY FUNCTIONS FOR OLD ERROR SYSTEM
// ============================================================================

// Forward declarations for compatibility
typedef struct ErrorSystem ErrorSystem;
struct Interpreter;  // Forward declaration only

/**
 * @brief Enable or disable colored error output
 * @param enable 1 to enable colors, 0 to disable
 */
void error_colors_enable(int enable);

/**
 * @brief Enable or disable stack traces in error output
 * @param system The error system instance
 * @param enable true to enable stack traces, false to disable
 */
void error_enable_stack_trace(EnhancedErrorSystem* system, bool enable);

/**
 * @brief Print the most recent error recorded in the global error system
 */
void error_print_last(void);

/**
 * @brief Get the global error system instance (legacy compatibility)
 * @return Pointer to the global error system cast as old ErrorSystem type
 */
ErrorSystem* error_system_get_global(void);

/**
 * @brief Enhanced error reporting for interpreter (compatibility function)
 * @param interpreter The interpreter instance
 * @param message The error message
 * @param line The line number
 * @param column The column number
 */
void interpreter_report_error_enhanced(struct Interpreter* interpreter, const char* message, int line, int column);

#endif // ENHANCED_ERROR_SYSTEM_H
