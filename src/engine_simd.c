/*
 * File: engine_simd.c
 * Description: The core counting engine. 
 *              Uses memory mapping because copying data to user space is for peasants.
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <immintrin.h> /* For both SSE and AVX intrinsics */
#include "lineblitz.h"

/* 
 * Map the file directly into virtual memory. 
 * Returns MAP_FAILED if the file is empty or unreachable.
 */
static const char* map_file(const char *filepath, size_t *size) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) return MAP_FAILED;

    struct stat st;
    if (fstat(fd, &st) < 0 || st.st_size == 0) {
        close(fd);
        return MAP_FAILED; 
    }

    *size = st.st_size;
    /* MAP_PRIVATE: We are just reading. No need to sync with the disk. */
    void *mapped = mmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd); /* File descriptor is no longer needed once mapped. */

    return (const char*)mapped;
}

/* 
 * Default 128-bit SSE2 Engine.
 * Processes 16 bytes per CPU cycle.
 */
uint64_t count_lines_sse(const char *filepath) {
    size_t size = 0;
    const char *data = map_file(filepath, &size);
    if (data == MAP_FAILED) return 0;

    uint64_t total_lines = 0;
    const char *ptr = data;
    const char *end = data + size;

    __m128i newline_vec = _mm_set1_epi8('\n');

    while (ptr + 16 <= end) {
        /* Load unaligned 16 bytes. */
        __m128i chunk = _mm_loadu_si128((const __m128i*)ptr);
        __m128i cmp = _mm_cmpeq_epi8(chunk, newline_vec);
        unsigned int mask = _mm_movemask_epi8(cmp);
        total_lines += __builtin_popcount(mask);
        ptr += 16;
    }

    /* The walk of shame: counting the remaining bytes one by one. */
    while (ptr < end) {
        if (*ptr == '\n') total_lines++;
        ptr++;
    }

    munmap((void*)data, size);
    return total_lines;
}

/* 
 * The 256-bit AVX2 Engine (Nitro Mode).
 * Processes 32 bytes per CPU cycle. 
 * The target attribute forces GCC to compile this function with AVX2 instructions.
 */
__attribute__((target("avx2")))
uint64_t count_lines_avx2(const char *filepath) {
    size_t size = 0;
    const char *data = map_file(filepath, &size);
    if (data == MAP_FAILED) return 0;

    uint64_t total_lines = 0;
    const char *ptr = data;
    const char *end = data + size;

    __m256i newline_vec = _mm256_set1_epi8('\n');

    while (ptr + 32 <= end) {
        __m256i chunk = _mm256_loadu_si256((const __m256i*)ptr);
        __m256i cmp = _mm256_cmpeq_epi8(chunk, newline_vec);
        unsigned int mask = _mm256_movemask_epi8(cmp);
        total_lines += __builtin_popcount(mask);
        ptr += 32;
    }

    while (ptr < end) {
        if (*ptr == '\n') total_lines++;
        ptr++;
    }

    munmap((void*)data, size);
    return total_lines;
}
