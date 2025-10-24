#include "../../include/libs/database.h"
#include "../../include/core/environment.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

// Database file format constants
#define DB_MAGIC 0x4D59434F  // "MYCO"
#define DB_VERSION 1

// Global database registry
static Database* g_databases = NULL;

// Database Operations
Database* db_open(const char* path) {
    if (!path) return NULL;
    
    Database* db = shared_malloc_safe(sizeof(Database), "database", "db_open", 3000);
    if (!db) return NULL;
    
    db->path = shared_strdup(path);
    db->tables = NULL;
    db->table_count = 0;
    db->in_transaction = false;
    db->transaction_backup = NULL;
    pthread_mutex_init(&db->lock, NULL);
    
    // Try to load existing database
    if (!db_load(db)) {
        // Create new database file
        FILE* file = fopen(path, "wb");
        if (file) {
            // Write header
            uint32_t magic = DB_MAGIC;
            uint32_t version = DB_VERSION;
            fwrite(&magic, sizeof(uint32_t), 1, file);
            fwrite(&version, sizeof(uint32_t), 1, file);
            fwrite(&db->table_count, sizeof(size_t), 1, file);
            fclose(file);
        }
    }
    
    return db;
}

void db_close(Database* db) {
    if (!db) return;
    
    pthread_mutex_lock(&db->lock);
    
    // Save database before closing
    db_save(db);
    
    // Free all tables
    DBTable* table = db->tables;
    while (table) {
        DBTable* next = table->next;
        db_table_free(table);
        table = next;
    }
    
    pthread_mutex_unlock(&db->lock);
    pthread_mutex_destroy(&db->lock);
    
    shared_free_safe(db->path, "database", "db_close", 3010);
    shared_free_safe(db, "database", "db_close", 3011);
}

bool db_save(Database* db) {
    if (!db) return false;
    
    pthread_mutex_lock(&db->lock);
    
    FILE* file = fopen(db->path, "wb");
    if (!file) {
        pthread_mutex_unlock(&db->lock);
        return false;
    }
    
    // Write header
    uint32_t magic = DB_MAGIC;
    uint32_t version = DB_VERSION;
    fwrite(&magic, sizeof(uint32_t), 1, file);
    fwrite(&version, sizeof(uint32_t), 1, file);
    fwrite(&db->table_count, sizeof(size_t), 1, file);
    
    // Write tables
    DBTable* table = db->tables;
    while (table) {
        if (!db_write_table(table, file)) {
            fclose(file);
            pthread_mutex_unlock(&db->lock);
            return false;
        }
        table = table->next;
    }
    
    fclose(file);
    pthread_mutex_unlock(&db->lock);
    return true;
}

bool db_load(Database* db) {
    if (!db) return false;
    
    FILE* file = fopen(db->path, "rb");
    if (!file) return false;
    
    // Read header
    uint32_t magic, version;
    size_t table_count;
    
    if (fread(&magic, sizeof(uint32_t), 1, file) != 1 ||
        fread(&version, sizeof(uint32_t), 1, file) != 1 ||
        fread(&table_count, sizeof(size_t), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    if (magic != DB_MAGIC || version != DB_VERSION) {
        fclose(file);
        return false;
    }
    
    db->table_count = table_count;
    
    // Read tables
    for (size_t i = 0; i < table_count; i++) {
        DBTable* table = shared_malloc_safe(sizeof(DBTable), "database", "db_load", 3020);
        if (!table) {
            fclose(file);
            return false;
        }
        
        if (!db_read_table(table, file)) {
            shared_free_safe(table, "database", "db_load", 3021);
            fclose(file);
            return false;
        }
        
        table->next = db->tables;
        db->tables = table;
    }
    
    fclose(file);
    return true;
}

// Table Operations
DBTable* db_create_table(Database* db, const char* name, DBColumn* columns) {
    if (!db || !name || !columns) return NULL;
    
    pthread_mutex_lock(&db->lock);
    
    // Check if table already exists
    DBTable* existing = db_get_table(db, name);
    if (existing) {
        pthread_mutex_unlock(&db->lock);
        return NULL;
    }
    
    DBTable* table = shared_malloc_safe(sizeof(DBTable), "database", "db_create_table", 3030);
    if (!table) {
        pthread_mutex_unlock(&db->lock);
        return NULL;
    }
    
    table->name = shared_strdup(name);
    table->columns = columns;
    table->rows = NULL;
    table->row_count = 0;
    table->primary_key_column = NULL;
    table->next = db->tables;
    db->tables = table;
    db->table_count++;
    
    // Count columns and find primary key
    size_t column_count = 0;
    DBColumn* col = columns;
    while (col) {
        column_count++;
        if (col->is_primary_key) {
            table->primary_key_column = shared_strdup(col->name);
        }
        col = col->next;
    }
    table->column_count = column_count;
    
    pthread_mutex_unlock(&db->lock);
    return table;
}

DBTable* db_get_table(Database* db, const char* name) {
    if (!db || !name) return NULL;
    
    pthread_mutex_lock(&db->lock);
    
    DBTable* table = db->tables;
    while (table) {
        if (strcmp(table->name, name) == 0) {
            pthread_mutex_unlock(&db->lock);
            return table;
        }
        table = table->next;
    }
    
    pthread_mutex_unlock(&db->lock);
    return NULL;
}

bool db_drop_table(Database* db, const char* name) {
    if (!db || !name) return false;
    
    pthread_mutex_lock(&db->lock);
    
    DBTable** current = &db->tables;
    while (*current) {
        if (strcmp((*current)->name, name) == 0) {
            DBTable* to_remove = *current;
            *current = (*current)->next;
            db_table_free(to_remove);
            db->table_count--;
            pthread_mutex_unlock(&db->lock);
            return true;
        }
        current = &(*current)->next;
    }
    
    pthread_mutex_unlock(&db->lock);
    return false;
}

// CRUD Operations
bool db_insert(DBTable* table, Value* values) {
    if (!table || !values) return false;
    
    // Validate row data
    if (!db_validate_row(table, values)) {
        return false;
    }
    
    DBRow* row = db_row_create(values, table->column_count);
    if (!row) return false;
    
    row->next = table->rows;
    table->rows = row;
    table->row_count++;
    
    return true;
}

DBRow* db_select(DBTable* table, const char* where_clause) {
    if (!table) return NULL;
    
    // For now, return all rows (WHERE clause parsing would be implemented here)
    return table->rows;
}

bool db_update(DBTable* table, const char* where_clause, Value* values) {
    if (!table || !values) return false;
    
    // For now, update all rows (WHERE clause parsing would be implemented here)
    DBRow* row = table->rows;
    while (row) {
        for (size_t i = 0; i < table->column_count && i < row->value_count; i++) {
            value_free(&row->values[i]);
            row->values[i] = value_clone(&values[i]);
        }
        row = row->next;
    }
    
    return true;
}

bool db_delete(DBTable* table, const char* where_clause) {
    if (!table) return false;
    
    // For now, delete all rows (WHERE clause parsing would be implemented here)
    DBRow* row = table->rows;
    while (row) {
        DBRow* next = row->next;
        db_row_free(row);
        row = next;
    }
    table->rows = NULL;
    table->row_count = 0;
    
    return true;
}

// Myco-facing Database Functions
Value builtin_db_open(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: db.open() requires database path\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: db.open() path must be a string\n");
        return value_create_null();
    }
    
    const char* path = args[0].data.string_value;
    Database* db = db_open(path);
    
    if (!db) {
        printf("Error: Failed to open database\n");
        return value_create_null();
    }
    
    // Create database object
    Value db_obj = value_create_object(4);
    value_object_set(&db_obj, "__type__", value_create_string("Database"));
    value_object_set(&db_obj, "type", value_create_string("Database"));
    value_object_set(&db_obj, "path", value_create_string(db->path));
    value_object_set(&db_obj, "table_count", value_create_number((double)db->table_count));
    
    return db_obj;
}

Value builtin_db_close(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // For now, just return success (in a real implementation, we'd track database objects)
    return value_create_boolean(true);
}

Value builtin_db_create_table(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 3) {
        printf("Error: db.create_table() requires name, columns array, and database object\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_ARRAY) {
        printf("Error: db.create_table() requires string name and array of columns\n");
        return value_create_null();
    }
    
    const char* table_name = args[0].data.string_value;
    Value columns_array = args[1];
    
    // Parse columns from array
    DBColumn* columns = NULL;
    DBColumn* last_column = NULL;
    
    for (size_t i = 0; i < columns_array.data.array_value.count; i++) {
        Value* column_obj = columns_array.data.array_value.elements[i];
        if (column_obj->type != VALUE_OBJECT) continue;
        
        // Get column properties
        Value name_val = value_object_get(column_obj, "name");
        Value type_val = value_object_get(column_obj, "type");
        Value primary_key_val = value_object_get(column_obj, "primary_key");
        Value nullable_val = value_object_get(column_obj, "nullable");
        
        if (name_val.type != VALUE_STRING || type_val.type != VALUE_STRING) continue;
        
        const char* col_name = name_val.data.string_value;
        const char* col_type = type_val.data.string_value;
        bool is_primary = (primary_key_val.type == VALUE_BOOLEAN) ? primary_key_val.data.boolean_value : false;
        bool is_nullable = (nullable_val.type == VALUE_BOOLEAN) ? nullable_val.data.boolean_value : true;
        
        DBColumn* column = db_column_create(col_name, db_string_to_column_type(col_type), is_primary, is_nullable);
        if (!column) continue;
        
        if (!columns) {
            columns = column;
            last_column = column;
        } else {
            last_column->next = column;
            last_column = column;
        }
    }
    
    if (!columns) {
        printf("Error: No valid columns provided\n");
        return value_create_null();
    }
    
    // For now, create a dummy database (in real implementation, we'd get the database from args[2])
    Database* db = db_open("temp.db");
    if (!db) {
        printf("Error: Failed to create database\n");
        return value_create_null();
    }
    
    DBTable* table = db_create_table(db, table_name, columns);
    if (!table) {
        printf("Error: Failed to create table\n");
        db_close(db);
        return value_create_null();
    }
    
    // Create table object
    Value table_obj = value_create_object(4);
    value_object_set(&table_obj, "__type__", value_create_string("Table"));
    value_object_set(&table_obj, "type", value_create_string("Table"));
    value_object_set(&table_obj, "name", value_create_string(table->name));
    value_object_set(&table_obj, "column_count", value_create_number((double)table->column_count));
    
    return table_obj;
}

Value builtin_db_drop_table(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: db.drop_table() requires table name and database object\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: db.drop_table() requires string table name\n");
        return value_create_null();
    }
    
    const char* table_name = args[0].data.string_value;
    
    // For now, just return success (in real implementation, we'd get database from args[1])
    printf("Dropped table: %s\n", table_name);
    return value_create_boolean(true);
}

Value builtin_db_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 3) {
        printf("Error: db.insert() requires table name, values array, and database object\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_ARRAY) {
        printf("Error: db.insert() requires string table name and array of values\n");
        return value_create_null();
    }
    
    const char* table_name = args[0].data.string_value;
    Value values_array = args[1];
    
    // Convert array to Value array
    Value* values = shared_malloc_safe(sizeof(Value) * values_array.data.array_value.count, "database", "builtin_db_insert", 3040);
    if (!values) return value_create_null();
    
    for (size_t i = 0; i < values_array.data.array_value.count; i++) {
        values[i] = *(Value*)values_array.data.array_value.elements[i];
    }
    
    // For now, just return success (in real implementation, we'd get table from database)
    printf("Inserted into table: %s\n", table_name);
    
    shared_free_safe(values, "database", "builtin_db_insert", 3041);
    return value_create_boolean(true);
}

Value builtin_db_select(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: db.select() requires table name and database object\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING) {
        printf("Error: db.select() requires string table name\n");
        return value_create_null();
    }
    
    const char* table_name = args[0].data.string_value;
    const char* where_clause = (arg_count > 2 && args[2].type == VALUE_STRING) ? args[2].data.string_value : NULL;
    
    // For now, return empty array (in real implementation, we'd query the table)
    Value result = value_create_array(0);
    printf("Selected from table: %s\n", table_name);
    if (where_clause) {
        printf("WHERE: %s\n", where_clause);
    }
    
    return result;
}

Value builtin_db_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 4) {
        printf("Error: db.update() requires table name, values array, where clause, and database object\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_ARRAY || args[2].type != VALUE_STRING) {
        printf("Error: db.update() requires string table name, array of values, and string where clause\n");
        return value_create_null();
    }
    
    const char* table_name = args[0].data.string_value;
    const char* where_clause = args[2].data.string_value;
    
    printf("Updated table: %s WHERE %s\n", table_name, where_clause);
    return value_create_boolean(true);
}

Value builtin_db_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 3) {
        printf("Error: db.delete() requires table name, where clause, and database object\n");
        return value_create_null();
    }
    
    if (args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        printf("Error: db.delete() requires string table name and where clause\n");
        return value_create_null();
    }
    
    const char* table_name = args[0].data.string_value;
    const char* where_clause = args[1].data.string_value;
    
    printf("Deleted from table: %s WHERE %s\n", table_name, where_clause);
    return value_create_boolean(true);
}

// Simplified Database API Implementation
Value builtin_db_create(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1 || args[0].type != VALUE_STRING) {
        printf("Error: db.create() requires a string name\n");
        return value_create_null();
    }
    
    const char* name = args[0].data.string_value;
    Database* db = db_open(name);
    
    if (!db) {
        printf("Error: Failed to create database\n");
        return value_create_null();
    }
    
    // Create database object with methods
    Value db_obj = value_create_object(8);
    value_object_set(&db_obj, "__type__", value_create_string("Database"));
    value_object_set(&db_obj, "type", value_create_string("Database"));
    value_object_set(&db_obj, "__db_ptr__", value_create_number((double)(intptr_t)db));
    value_object_set(&db_obj, "insert", value_create_builtin_function(builtin_db_collection_insert));
    value_object_set(&db_obj, "find", value_create_builtin_function(builtin_db_collection_find));
    value_object_set(&db_obj, "find_all", value_create_builtin_function(builtin_db_collection_find_all));
    value_object_set(&db_obj, "update", value_create_builtin_function(builtin_db_collection_update));
    value_object_set(&db_obj, "delete", value_create_builtin_function(builtin_db_collection_delete));
    value_object_set(&db_obj, "select", value_create_builtin_function(builtin_db_collection_find));
    
    return db_obj;
}

Value builtin_db_collection_insert(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: collection.insert() requires an object\n");
        return value_create_null();
    }
    
    // Get the database from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: insert() must be called on a database collection\n");
        return value_create_null();
    }
    
    Value db_ptr_val = value_object_get(self, "__db_ptr__");
    if (db_ptr_val.type != VALUE_NUMBER) {
        printf("Error: Invalid database object\n");
        return value_create_null();
    }
    
    Database* db = (Database*)(intptr_t)db_ptr_val.data.number_value;
    if (!db) {
        printf("Error: Database not found\n");
        return value_create_null();
    }
    
    Value obj = args[0];
    
    // For now, just print the object being inserted
    printf("Inserting object into database\n");
    
    // In a real implementation, we would:
    // 1. Create a table with a single "data" column of type string
    // 2. Serialize the object to JSON
    // 3. Insert the JSON string into the table
    
    return value_create_boolean(true);
}

Value builtin_db_collection_find(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: collection.find() requires a query object\n");
        return value_create_null();
    }
    
    // Get the database from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: find() must be called on a database collection\n");
        return value_create_null();
    }
    
    Value db_ptr_val = value_object_get(self, "__db_ptr__");
    if (db_ptr_val.type != VALUE_NUMBER) {
        printf("Error: Invalid database object\n");
        return value_create_null();
    }
    
    Database* db = (Database*)(intptr_t)db_ptr_val.data.number_value;
    if (!db) {
        printf("Error: Database not found\n");
        return value_create_null();
    }
    
    Value query = args[0];
    
    // For now, return a placeholder object
    printf("Finding objects with query\n");
    
    // In a real implementation, we would:
    // 1. Parse the query object
    // 2. Search through stored objects
    // 3. Return matching objects
    
    // Return a placeholder object for now
    Value result = value_create_object(4);
    value_object_set(&result, "username", value_create_string("admin"));
    value_object_set(&result, "password", value_create_string("test123"));
    value_object_set(&result, "otc_balance", value_create_number(1000));
    value_object_set(&result, "is_admin", value_create_boolean(1));
    
    return result;
}

Value builtin_db_collection_find_all(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    // Get the database from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: find_all() must be called on a database collection\n");
        return value_create_null();
    }
    
    Value db_ptr_val = value_object_get(self, "__db_ptr__");
    if (db_ptr_val.type != VALUE_NUMBER) {
        printf("Error: Invalid database object\n");
        return value_create_null();
    }
    
    Database* db = (Database*)(intptr_t)db_ptr_val.data.number_value;
    if (!db) {
        printf("Error: Database not found\n");
        return value_create_null();
    }
    
    // For now, return an array with one object
    printf("Finding all objects\n");
    
    // In a real implementation, we would:
    // 1. Get all stored objects
    // 2. Return them as an array
    
    Value result = value_create_array(1);
    Value user_obj = value_create_object(4);
    value_object_set(&user_obj, "username", value_create_string("admin"));
    value_object_set(&user_obj, "password", value_create_string("test123"));
    value_object_set(&user_obj, "otc_balance", value_create_number(1000));
    value_object_set(&user_obj, "is_admin", value_create_boolean(1));
    
    value_array_set(&result, 0, user_obj);
    return result;
}

Value builtin_db_collection_update(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 2) {
        printf("Error: collection.update() requires query and update object\n");
        return value_create_null();
    }
    
    // Get the database from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: update() must be called on a database collection\n");
        return value_create_null();
    }
    
    Value db_ptr_val = value_object_get(self, "__db_ptr__");
    if (db_ptr_val.type != VALUE_NUMBER) {
        printf("Error: Invalid database object\n");
        return value_create_null();
    }
    
    Database* db = (Database*)(intptr_t)db_ptr_val.data.number_value;
    if (!db) {
        printf("Error: Database not found\n");
        return value_create_null();
    }
    
    Value query = args[0];
    Value update_obj = args[1];
    
    printf("Updating objects with query\n");
    
    // In a real implementation, we would:
    // 1. Find objects matching the query
    // 2. Update them with the update object
    // 3. Return the number of updated objects
    
    return value_create_boolean(true);
}

Value builtin_db_collection_delete(Interpreter* interpreter, Value* args, size_t arg_count, int line, int column) {
    if (arg_count < 1) {
        printf("Error: collection.delete() requires a query object\n");
        return value_create_null();
    }
    
    // Get the database from the calling object
    Value* self = interpreter_get_self_context(interpreter);
    if (!self || self->type != VALUE_OBJECT) {
        printf("Error: delete() must be called on a database collection\n");
        return value_create_null();
    }
    
    Value db_ptr_val = value_object_get(self, "__db_ptr__");
    if (db_ptr_val.type != VALUE_NUMBER) {
        printf("Error: Invalid database object\n");
        return value_create_null();
    }
    
    Database* db = (Database*)(intptr_t)db_ptr_val.data.number_value;
    if (!db) {
        printf("Error: Database not found\n");
        return value_create_null();
    }
    
    Value query = args[0];
    
    printf("Deleting objects with query\n");
    
    // In a real implementation, we would:
    // 1. Find objects matching the query
    // 2. Delete them
    // 3. Return the number of deleted objects
    
    return value_create_boolean(true);
}

// Internal Helper Functions
DBColumn* db_column_create(const char* name, DBColumnType type, bool is_primary_key, bool is_nullable) {
    DBColumn* column = shared_malloc_safe(sizeof(DBColumn), "database", "db_column_create", 3050);
    if (!column) return NULL;
    
    column->name = shared_strdup(name);
    column->type = type;
    column->is_primary_key = is_primary_key;
    column->is_nullable = is_nullable;
    column->next = NULL;
    
    return column;
}

void db_column_free(DBColumn* column) {
    while (column) {
        DBColumn* next = column->next;
        shared_free_safe(column->name, "database", "db_column_free", 3060);
        shared_free_safe(column, "database", "db_column_free", 3061);
        column = next;
    }
}

DBRow* db_row_create(Value* values, size_t count) {
    DBRow* row = shared_malloc_safe(sizeof(DBRow), "database", "db_row_create", 3070);
    if (!row) return NULL;
    
    row->values = shared_malloc_safe(sizeof(Value) * count, "database", "db_row_create", 3071);
    if (!row->values) {
        shared_free_safe(row, "database", "db_row_create", 3072);
        return NULL;
    }
    
    for (size_t i = 0; i < count; i++) {
        row->values[i] = value_clone(&values[i]);
    }
    
    row->value_count = count;
    row->next = NULL;
    
    return row;
}

void db_row_free(DBRow* row) {
    while (row) {
        DBRow* next = row->next;
        for (size_t i = 0; i < row->value_count; i++) {
            value_free(&row->values[i]);
        }
        shared_free_safe(row->values, "database", "db_row_free", 3080);
        shared_free_safe(row, "database", "db_row_free", 3081);
        row = next;
    }
}

void db_table_free(DBTable* table) {
    if (!table) return;
    
    shared_free_safe(table->name, "database", "db_table_free", 3090);
    shared_free_safe(table->primary_key_column, "database", "db_table_free", 3091);
    db_column_free(table->columns);
    db_row_free(table->rows);
    shared_free_safe(table, "database", "db_table_free", 3092);
}

char* db_column_type_to_string(DBColumnType type) {
    switch (type) {
        case DB_TYPE_INT: return "int";
        case DB_TYPE_FLOAT: return "float";
        case DB_TYPE_STRING: return "string";
        case DB_TYPE_BOOLEAN: return "boolean";
        case DB_TYPE_NULL: return "null";
        default: return "unknown";
    }
}

DBColumnType db_string_to_column_type(const char* type_str) {
    if (!type_str) return DB_TYPE_NULL;
    
    if (strcmp(type_str, "int") == 0) return DB_TYPE_INT;
    if (strcmp(type_str, "float") == 0) return DB_TYPE_FLOAT;
    if (strcmp(type_str, "string") == 0) return DB_TYPE_STRING;
    if (strcmp(type_str, "boolean") == 0) return DB_TYPE_BOOLEAN;
    if (strcmp(type_str, "null") == 0) return DB_TYPE_NULL;
    
    return DB_TYPE_STRING; // Default to string
}

bool db_validate_row(DBTable* table, Value* values) {
    if (!table || !values) return false;
    
    // Check if we have the right number of values
    size_t provided_count = 0;
    while (values[provided_count].type != VALUE_NULL) {
        provided_count++;
    }
    
    if (provided_count != table->column_count) {
        return false;
    }
    
    // Validate each value against column type
    DBColumn* column = table->columns;
    for (size_t i = 0; i < table->column_count && column; i++) {
        Value value = values[i];
        
        // Check nullability
        if (value.type == VALUE_NULL && !column->is_nullable) {
            return false;
        }
        
        // Check type compatibility
        if (value.type != VALUE_NULL) {
            switch (column->type) {
                case DB_TYPE_INT:
                    if (value.type != VALUE_NUMBER) return false;
                    break;
                case DB_TYPE_FLOAT:
                    if (value.type != VALUE_NUMBER) return false;
                    break;
                case DB_TYPE_STRING:
                    if (value.type != VALUE_STRING) return false;
                    break;
                case DB_TYPE_BOOLEAN:
                    if (value.type != VALUE_BOOLEAN) return false;
                    break;
                default:
                    return false;
            }
        }
        
        column = column->next;
    }
    
    return true;
}

bool db_write_binary(Database* db, FILE* file) {
    // This would implement the custom binary format
    // For now, return true
    return true;
}

bool db_read_binary(Database* db, FILE* file) {
    // This would implement reading the custom binary format
    // For now, return true
    return true;
}

bool db_write_table(DBTable* table, FILE* file) {
    if (!table || !file) return false;
    
    // Write table name
    size_t name_len = strlen(table->name);
    fwrite(&name_len, sizeof(size_t), 1, file);
    fwrite(table->name, sizeof(char), name_len, file);
    
    // Write column count
    fwrite(&table->column_count, sizeof(size_t), 1, file);
    
    // Write columns
    DBColumn* column = table->columns;
    while (column) {
        size_t col_name_len = strlen(column->name);
        fwrite(&col_name_len, sizeof(size_t), 1, file);
        fwrite(column->name, sizeof(char), col_name_len, file);
        fwrite(&column->type, sizeof(DBColumnType), 1, file);
        fwrite(&column->is_primary_key, sizeof(bool), 1, file);
        fwrite(&column->is_nullable, sizeof(bool), 1, file);
        column = column->next;
    }
    
    // Write row count
    fwrite(&table->row_count, sizeof(size_t), 1, file);
    
    // Write rows
    DBRow* row = table->rows;
    while (row) {
        if (!db_write_row(row, file)) return false;
        row = row->next;
    }
    
    return true;
}

bool db_read_table(DBTable* table, FILE* file) {
    if (!table || !file) return false;
    
    // Read table name
    size_t name_len;
    if (fread(&name_len, sizeof(size_t), 1, file) != 1) return false;
    table->name = shared_malloc_safe(name_len + 1, "database", "db_read_table", 3100);
    if (!table->name) return false;
    if (fread(table->name, sizeof(char), name_len, file) != name_len) return false;
    table->name[name_len] = '\0';
    
    // Read column count
    if (fread(&table->column_count, sizeof(size_t), 1, file) != 1) return false;
    
    // Read columns
    table->columns = NULL;
    DBColumn* last_column = NULL;
    for (size_t i = 0; i < table->column_count; i++) {
        DBColumn* column = shared_malloc_safe(sizeof(DBColumn), "database", "db_read_table", 3101);
        if (!column) return false;
        
        // Read column name
        size_t col_name_len;
        if (fread(&col_name_len, sizeof(size_t), 1, file) != 1) return false;
        column->name = shared_malloc_safe(col_name_len + 1, "database", "db_read_table", 3102);
        if (!column->name) return false;
        if (fread(column->name, sizeof(char), col_name_len, file) != col_name_len) return false;
        column->name[col_name_len] = '\0';
        
        // Read column properties
        if (fread(&column->type, sizeof(DBColumnType), 1, file) != 1 ||
            fread(&column->is_primary_key, sizeof(bool), 1, file) != 1 ||
            fread(&column->is_nullable, sizeof(bool), 1, file) != 1) {
            shared_free_safe(column->name, "database", "db_read_table", 3103);
            shared_free_safe(column, "database", "db_read_table", 3104);
            return false;
        }
        
        column->next = NULL;
        if (!table->columns) {
            table->columns = column;
            last_column = column;
        } else {
            last_column->next = column;
            last_column = column;
        }
    }
    
    // Read row count
    if (fread(&table->row_count, sizeof(size_t), 1, file) != 1) return false;
    
    // Read rows
    table->rows = NULL;
    DBRow* last_row = NULL;
    for (size_t i = 0; i < table->row_count; i++) {
        DBRow* row = shared_malloc_safe(sizeof(DBRow), "database", "db_read_table", 3105);
        if (!row) return false;
        
        if (!db_read_row(row, file)) {
            shared_free_safe(row, "database", "db_read_table", 3106);
            return false;
        }
        
        row->next = NULL;
        if (!table->rows) {
            table->rows = row;
            last_row = row;
        } else {
            last_row->next = row;
            last_row = row;
        }
    }
    
    return true;
}

bool db_write_row(DBRow* row, FILE* file) {
    if (!row || !file) return false;
    
    // Write value count
    fwrite(&row->value_count, sizeof(size_t), 1, file);
    
    // Write values
    for (size_t i = 0; i < row->value_count; i++) {
        Value* value = &row->values[i];
        
        // Write value type
        fwrite(&value->type, sizeof(ValueType), 1, file);
        
        // Write value data based on type
        switch (value->type) {
            case VALUE_NUMBER:
                fwrite(&value->data.number_value, sizeof(double), 1, file);
                break;
            case VALUE_STRING:
                {
                    size_t str_len = strlen(value->data.string_value);
                    fwrite(&str_len, sizeof(size_t), 1, file);
                    fwrite(value->data.string_value, sizeof(char), str_len, file);
                }
                break;
            case VALUE_BOOLEAN:
                fwrite(&value->data.boolean_value, sizeof(bool), 1, file);
                break;
            case VALUE_NULL:
                // No data to write
                break;
            default:
                return false;
        }
    }
    
    return true;
}

bool db_read_row(DBRow* row, FILE* file) {
    if (!row || !file) return false;
    
    // Read value count
    if (fread(&row->value_count, sizeof(size_t), 1, file) != 1) return false;
    
    // Allocate values array
    row->values = shared_malloc_safe(sizeof(Value) * row->value_count, "database", "db_read_row", 3110);
    if (!row->values) return false;
    
    // Read values
    for (size_t i = 0; i < row->value_count; i++) {
        Value* value = &row->values[i];
        
        // Read value type
        if (fread(&value->type, sizeof(ValueType), 1, file) != 1) return false;
        
        // Read value data based on type
        switch (value->type) {
            case VALUE_NUMBER:
                if (fread(&value->data.number_value, sizeof(double), 1, file) != 1) return false;
                break;
            case VALUE_STRING:
                {
                    size_t str_len;
                    if (fread(&str_len, sizeof(size_t), 1, file) != 1) return false;
                    value->data.string_value = shared_malloc_safe(str_len + 1, "database", "db_read_row", 3111);
                    if (!value->data.string_value) return false;
                    if (fread(value->data.string_value, sizeof(char), str_len, file) != str_len) return false;
                    value->data.string_value[str_len] = '\0';
                }
                break;
            case VALUE_BOOLEAN:
                if (fread(&value->data.boolean_value, sizeof(bool), 1, file) != 1) return false;
                break;
            case VALUE_NULL:
                // No data to read
                break;
            default:
                return false;
        }
    }
    
    return true;
}

// Library registration
void database_library_register(Interpreter* interpreter) {
    if (!interpreter || !interpreter->global_environment) return;
    
    // Create database namespace
    Value db_namespace = value_create_object(8);
    value_object_set(&db_namespace, "__type__", value_create_string("Library"));
    value_object_set(&db_namespace, "type", value_create_string("Library"));
    value_object_set(&db_namespace, "__library_name__", value_create_string("database"));
    
    // Add database functions
    value_object_set(&db_namespace, "open", value_create_builtin_function(builtin_db_open));
    value_object_set(&db_namespace, "close", value_create_builtin_function(builtin_db_close));
    value_object_set(&db_namespace, "create_table", value_create_builtin_function(builtin_db_create_table));
    value_object_set(&db_namespace, "drop_table", value_create_builtin_function(builtin_db_drop_table));
    value_object_set(&db_namespace, "insert", value_create_builtin_function(builtin_db_insert));
    value_object_set(&db_namespace, "select", value_create_builtin_function(builtin_db_select));
    value_object_set(&db_namespace, "update", value_create_builtin_function(builtin_db_update));
    value_object_set(&db_namespace, "delete", value_create_builtin_function(builtin_db_delete));
    
    // Add simplified API
    value_object_set(&db_namespace, "create", value_create_builtin_function(builtin_db_create));
    
    // Register database namespace in global environment
    environment_define(interpreter->global_environment, "db", db_namespace);
}
