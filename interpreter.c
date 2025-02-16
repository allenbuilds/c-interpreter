#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "common.h"

/*----------memory.h-------------*/
#include <stdio.h>
#include "memory.h"
#include "value.h"

void initValueArray(ValueArray* array) {
  array->values = NULL;
  array->capacity = 0;
  array->count = 0;
}


/* ---------- chunk.h ---------- */
#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"

typedef enum {
    OP_RETURN,
} OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte);

#endif

/* ---------- memory.h ---------- */
#ifndef clox_memory_h
#define clox_memory_h

#include "common.h"

typedef double Value;

typedef struct {
    int capacity;
    int count;
    Value* values;
  } ValueArray;
  
  void initValueArray(ValueArray* array);
  void writeValueArray(ValueArray* array, Value value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity; 
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
  }






  void freeValueArray(ValueArray* array);

#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))
#define FREE_ARRAY(type, pointer, oldCount) \
    reallocate(pointer, sizeof(type) * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif

/* ---------- debug.h ---------- */
#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int offset);

#endif

/* ---------- memory.c implementation ---------- */
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }
    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}

/* ---------- chunk.c implementation ---------- */
void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
}

void writeChunk(Chunk* chunk, uint8_t byte) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    initChunk(chunk);
}

/* ---------- debug.c implementation ---------- */
static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

int disassembleInstruction(Chunk* chunk, int offset) {
    printf("%04d ", offset);
    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}

void disassembleChunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}







/* ---------- main ---------- */
int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);
    writeChunk(&chunk, OP_RETURN);
    
    /* Disassemble before freeing the chunk data */
    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);

    return 0;
}