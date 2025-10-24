#ifndef MYCO_DATABASE_H
#define MYCO_DATABASE_H

#include "../core/core.h"
#include "../core/interpreter/value_operations.h"
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

// Database Column Types
typedef enum {
    DB_TYPE_INT,
    DB_TYPE_FLOAT,
    DB_TYPE_STRING,
    DB_TYPE_BOOLEAN,
    DB_TYPE_NULL
} DBColumnType;

// Database Column Definition
typedef struct DBColumn {
    char* name;
    DBColumnType type;
    bool is_primary_key;
    bool is_nullable;
    struct DBColumn* next;
} DBColumn;

// Database Row
typedef struct DBRow {
    Value* values;
    size_t value_count;
    struct DBRow* next;
} DBRow;

// Database Table
typedef struct DBTable {
    char* name;
    DBColumn* columns;
    size_t column_count;
    DBRow* rows;
    size_t row_count;
    char* primary_key_column;
    struct DBTable* next;
} DBTable;

// Database Instance
typedef struct Database {
    char* path;
    DBTable* tables;
    size_t table_count;
    bool in_transaction;
    DBRow* transaction_backup;
    pthread_mutex_t lock;
} Database;

// Database Operations
Database* db_open(const char* path);
void db_close(Database* db);
bool db_save(Database* db);
bool db_load(Database* db);

// Table Operations
DBTable* db_create_table(Database* db, const char* name, DBColumn* columns);
DBTable* db_get_table(Database* db, const char* name);
bool db_drop_table(Database* db, const char* name);

// CRUD Operations
bool db_insert(DBTable* table, Value* values);
DBRow* db_select(DBTable* table, const char* where_clause);
bool db_update(DBTable* table, const char* where_clause, Value* values);
bool db_delete(DBTable* table, const char* where_clause);

// Myco-facing Database Functions
Value builtin_db_open(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_create_table(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_drop_table(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_select(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Simplified Database API
Value builtin_db_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_collection_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_collection_find(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_collection_find_all(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_collection_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);
Value builtin_db_collection_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column);

// Internal Helper Functions
DBColumn* db_column_create(const char* name, DBColumnType type, bool is_primary_key, bool is_nullable);
void db_column_free(DBColumn* column);
DBRow* db_row_create(Value* values, size_t count);
void db_row_free(DBRow* row);
void db_table_free(DBTable* table);
char* db_column_type_to_string(DBColumnType type);
DBColumnType db_string_to_column_type(const char* type_str);
bool db_validate_row(DBTable* table, Value* values);
bool db_write_binary(Database* db, FILE* file);
bool db_read_binary(Database* db, FILE* file);
bool db_write_table(DBTable* table, FILE* file);
bool db_read_table(DBTable* table, FILE* file);
bool db_write_row(DBRow* row, FILE* file);
bool db_read_row(DBRow* row, FILE* file);

// Library registration
void database_library_register(Interpreter* interpreter);

#endif // MYCO_DATABASE_H
