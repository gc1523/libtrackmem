#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../include/trackmem.h"

void test_malloc_free() {
    printf("Running test_malloc_free...\n");
    char *s = malloc(100);
    assert(s != NULL);
    strcpy(s, "Hello TrackMem");
    assert(strcmp(s, "Hello TrackMem") == 0);
    free(s);
    printf("Passed test_malloc_free.\n");
}

void test_calloc_free() {
    printf("Running test_calloc_free...\n");
    int *arr = calloc(10, sizeof(int));
    assert(arr != NULL);
    for (int i = 0; i < 10; i++) {
        arr[i] = i;
    }
    for (int i = 0; i < 10; i++) {
        assert(arr[i] == i);
    }
    free(arr);
    printf("Passed test_calloc_free.\n");
}

void test_realloc() {
    printf("Running test_realloc...\n");
    int *arr = malloc(5 * sizeof(int));
    assert(arr != NULL);
    for (int i = 0; i < 5; i++) {
        arr[i] = i;
    }
    arr = realloc(arr, 10 * sizeof(int));
    assert(arr != NULL);
    for (int i = 0; i < 5; i++) {
        assert(arr[i] == i);
    }
    for (int i = 5; i < 10; i++) {
        arr[i] = i;
    }
    for (int i = 5; i < 10; i++) {
        assert(arr[i] == i);
    }
    free(arr);
    printf("Passed test_realloc.\n");
}


void test_leak_detection() {
    printf("Running test_leak_detection (intentional leak)...\n");
    char *leaky = malloc(50);
    assert(leaky != NULL);
    strcpy(leaky, "This memory is leaked!");
    printf("Passed test_leak_detection. (Check log output for leak report.)\n");
}

void test_free_untracked() {
    printf("Running test_free_untracked...\n");
    void* untracked = (void *) 0xdeadbeef;
    free(untracked);
    printf("Passed test_free_untracked.\n");
}

int main() {
    test_malloc_free();
    test_calloc_free();
    test_realloc();
    test_leak_detection();
    test_free_untracked();
    printf("All tests passed.\n");
    return 0;
}