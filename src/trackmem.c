#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct allocation_record {
  void *ptr;
  size_t size;
  char file[256];
  int line;
  char func[256];
  struct allocation_record *next;
} allocation_record;



static allocation_record *allocations = NULL;
FILE *log_file = NULL;
char * log_file_name = "trackmem.log";
bool init = false;

static void add_record(void *ptr, size_t size, const char *file, int line, const char *func) {
  allocation_record *record = malloc(sizeof(allocation_record));
  if (!record) {
      fprintf(stderr, "Failed to allocate memory for allocation record.\n");
      exit(EXIT_FAILURE);
  }
  record->ptr = ptr;
  record->size = size;
  strncpy(record->file, file, sizeof(record->file) - 1);
  record->file[sizeof(record->file) - 1] = '\0';
  record->line = line;
  strncpy(record->func, func, sizeof(record->func) - 1);
  record->func[sizeof(record->func) - 1] = '\0';
  record->next = allocations;
  allocations = record;
}

static void remove_record(void *ptr) {
  allocation_record *current = allocations;
  allocation_record *prev = NULL;
  while (current) {
      if (current->ptr == ptr) {
          if (prev) {
              prev->next = current->next;
          } else {
              allocations = current->next;
          }
          free(current);
          return;
      }
      prev = current;
      current = current->next;
  }
  fprintf(stderr, "Attempting to free untracked memory: %p\n", ptr);
}

static allocation_record *find_record(void *ptr) {
  allocation_record *current = allocations;
  while (current) {
      if (current->ptr == ptr) {
          return current;
      }
      current = current->next;
  }
  return NULL;
}

void trackmem_init() {
    if (init) {
        fprintf(stderr, "trackmem_init() called multiple times\n");
        exit(EXIT_FAILURE);
    } else {
        init = true;
    }

    log_file = fopen(log_file_name, "w");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file: %s\n", log_file_name);
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Tracked malloc call.
 *
 * This is the libtrackmem wrapper for malloc. It tracks the memory allocation in
 * a log file and stores the allocation record, including the file, line number,
 * and function name where the allocation occurred.
 *
 * @param size The size of allocated memory space.
 * @param file The file in which the malloc was called.
 * @param line The line number in the file where the malloc was called.
 * @param func The function in which the malloc was called.
 * @return Pointer to allocated memory space - NULL if malloc fails.
 */
void * t_malloc(size_t size, const char *file, int line, const char *func) {
  if (init) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed in %s at %s:%d\n", func, file, line);
        exit(EXIT_FAILURE);
    }
    add_record(ptr, size, file, line, func);
    fprintf(log_file, "malloc(%zu) at %s:%d in %s: %p\n", size, file, line, func, ptr);
    return ptr;
  } else {
    return malloc(size);
  } 
}

/**
 * @brief Tracked calloc call.
 *
 * This is the libtrackmem wrapper for calloc. It tracks the memory allocation in
 * a log file and stores the allocation record, including the file, line number,
 * and function name where the allocation occurred.
 * 
 * @param num The number of elements to allocate.
 * @param size The size of each element.
 * @param file The file in which the malloc was called.
 * @param line The line number in the file where the malloc was called.
 * @param func The function in which the malloc was called.
 * @return Pointer to allocated memory space - NULL if calloc fails.
 */
void * t_calloc(size_t num, size_t size, const char *file, int line, const char *func) {
  if (init) {
    void *ptr = calloc(num, size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed in %s at %s:%d\n", func, file, line);
        exit(EXIT_FAILURE);
    }
    add_record(ptr, num * size, file, line, func);
    fprintf(log_file, "calloc(%zu, %zu) at %s:%d in %s: %p\n", num, size, file, line, func, ptr);
    return ptr;
  } else {
    return calloc(num, size);
  } 
}

/**
 * @brief Tracked realloc call.
 *
 * This is the libtrackmem wrapper for realloc. It updates the allocation record of the
 * memory block being reallocated. If the pointer is NULL, it behaves like a malloc.
 *
 * @param ptr Pointer to the memory block to be reallocated.
 * @param size The new size of the memory block.
 * @param file The file in which the malloc was called.
 * @param line The line number in the file where the malloc was called.
 * @param func The function in which the malloc was called.
 * @return Pointer to reallocated memory space - NULL if realloc fails.
 */
void * t_realloc(void *ptr, size_t size, const char *file, int line, const char *func) {
  if (init) {
      if (ptr == NULL) {
          // realloc behaves like malloc when ptr is NULL
          void *new_ptr = malloc(size);
          if (new_ptr == NULL) {
              fprintf(stderr, "Memory allocation failed in %s at %s:%d\n", func, file, line);
              exit(EXIT_FAILURE);
          }
          add_record(new_ptr, size, file, line, func);
          fprintf(log_file, "realloc(NULL, %zu) at %s:%d in %s: %p\n", size, file, line, func, new_ptr);
          return new_ptr;
      }

      allocation_record *record = find_record(ptr);
      if (!record) {
          fprintf(stderr, "realloc on untracked memory %p in %s at %s:%d\n", ptr, func, file, line);
          exit(EXIT_FAILURE);
      }
      
      void *new_ptr = realloc(ptr, size);
      if (new_ptr == NULL) {
          fprintf(stderr, "Memory allocation failed in %s at %s:%d\n", func, file, line);
          exit(EXIT_FAILURE);
      }
      
      // Update record with new pointer and size
      record->ptr = new_ptr;
      record->size = size;
      strncpy(record->file, file, sizeof(record->file) - 1);
      record->file[sizeof(record->file) - 1] = '\0';
      record->line = line;
      strncpy(record->func, func, sizeof(record->func) - 1);
      record->func[sizeof(record->func) - 1] = '\0';
      
      fprintf(log_file, "realloc(%p, %zu) at %s:%d in %s: %p\n", ptr, size, file, line, func, new_ptr);
      return new_ptr;
  } else {
      return realloc(ptr, size);
  }
}
/**
 * @brief Tracked free call.
 *
 * This is the libtrackmem wrapper for free. It removes the allocation record
 * associated with the pointer being freed. If the pointer is NULL, it behaves
 * like a no-op.
 *
 * @param ptr Pointer to the memory block to be freed.
 * @param file The file in which the malloc was called.
 * @param line The line number in the file where the malloc was called.
 * @param func The function in which the malloc was called.
 * @return None.
 */
void t_free(void *ptr, const char *file, int line, const char *func) {
  if (init) {
    fprintf(log_file, "free(%p) at %s:%d in %s\n", ptr, file, line, func);
    remove_record(ptr);
    free(ptr);
  } else {
    free(ptr);
  } 
}

void clean_up (void) {
  if (!init) {
      return;
  }
  allocation_record *current = allocations;
  while (current) {
      fprintf(log_file, "Memory leak detected: %p allocated at %s:%d in %s, size: %zu\n",
              current->ptr, current->file, current->line, current->func, current->size);
      printf("Memory leak detected: %p allocated at %s:%d in %s, size: %zu\n",
                current->ptr, current->file, current->line, current->func, current->size);
        
      allocation_record *temp = current;
      current = current->next;
      free(temp);
  }
  printf("📁 See log file: ./%s\n", log_file_name);
  fclose(log_file);
}
/**
 * @brief Constructor function to initialize the library.
 * 
 * This function is called automatically when the library is loaded.
 * It initialises the log file and sets up the memory tracking system.
 * 
 */
__attribute__((constructor))
static void trackmem_constructor() {
    trackmem_init();
    atexit(clean_up);
}