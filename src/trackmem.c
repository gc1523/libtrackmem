#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct AllocationRecord {
  void *ptr;
  size_t size;
  char file[256];
  int line;
  char func[256];
  struct AllocationRecord *next;
} AllocationRecord;



static AllocationRecord *allocations = NULL;
FILE *log_file = NULL;
char * log_file_name = "trackmem.log";
bool init = false;

static void add_record(void *ptr, size_t size, const char *file, int line, const char *func) {
  AllocationRecord *record = malloc(sizeof(AllocationRecord));
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
  AllocationRecord *current = allocations;
  AllocationRecord *prev = NULL;
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

static AllocationRecord *find_record(void *ptr) {
  AllocationRecord *current = allocations;
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

      AllocationRecord *record = find_record(ptr);
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
  AllocationRecord *current = allocations;
  while (current) {
      fprintf(log_file, "Memory leak detected: %p allocated at %s:%d in %s, size: %zu\n",
              current->ptr, current->file, current->line, current->func, current->size);
      printf("Memory leak detected: %p allocated at %s:%d in %s, size: %zu\n",
                current->ptr, current->file, current->line, current->func, current->size);
        
      AllocationRecord *temp = current;
      current = current->next;
      free(temp);
  }
  printf("📁 See log file: ./%s\n", log_file_name);
  fclose(log_file);
}

__attribute__((constructor))
static void trackmem_constructor() {
    trackmem_init();
    atexit(clean_up);
}