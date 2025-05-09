#ifndef TRACKMEM_H
#define TRACKMEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>

#define malloc(size) t_malloc(size, __FILE__, __LINE__, __func__)
#define calloc(num, size) t_calloc(num, size, __FILE__, __LINE__, __func__)
#define realloc(ptr, size) t_realloc(ptr, size, __FILE__, __LINE__, __func__)
#define free(ptr) t_free(ptr, __FILE__, __LINE__, __func__)

void * t_malloc(size_t size, const char *file, int line, const char *func);
void * t_calloc(size_t num, size_t size, const char *file, int line, const char *func);
void * t_realloc(void *ptr, size_t size, const char *file, int line, const char *func);
void t_free(void *ptr, const char *file, int line, const char *func);

#ifdef __cplusplus
}
#endif

#endif