/*
 * File: lineblitz.h
 * Description: The grand architectural blueprint. 
 *              Defines the rules of engagement for counting lines at light speed.
 */

#ifndef LINEBLITZ_H
#define LINEBLITZ_H

#include <stdbool.h>
#include <stdint.h>

/* 
 * Configuration struct. 
 * Stores user preferences so we don't have to pass 50 arguments 
 * like we're coding in the 70s.
 */
typedef struct {
    bool force;         /* -f : Embrace the chaos. Count everything. */
    bool summary;       /* -s : Mute the noise. Show only the final verdict. */
    bool blitz;         /* -b : Nitro mode. AVX2 or bust. */
    const char *target_path;
} AppConfig;

/* 
 * Stats struct. 
 * Keeping track of our accomplishments (or lack thereof).
 */
typedef struct {
    uint64_t total_lines;
    uint32_t total_files;
    double elapsed_time_sec;
} Stats;

/* --- Core Functions --- */

/* 
 * The Directory Scout. 
 * Navigates the filesystem and delegates counting to the SIMD engines.
 */
void discover_and_count(AppConfig *config, Stats *stats);

/* --- SIMD Engines --- */

/* 128-bit SSE2 Engine. Fast. Reliable. Classic. */
uint64_t count_lines_sse(const char *filepath);

/* 256-bit AVX2 Engine. For when SSE2 just isn't fast enough for your ego. */
uint64_t count_lines_avx2(const char *filepath);

/* --- Utility Functions --- */

/* 
 * Hardware interrogation. 
 * Checks if the CPU actually supports AVX2 before we let it try.
 * Prevents the program from exploding on older silicon.
 */
void enforce_hardware_requirements(AppConfig *config);

#endif /* LINEBLITZ_H */
