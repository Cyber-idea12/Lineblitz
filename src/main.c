/*
 * File: main.c
 * Description: Entry point. Parses flags and handles users who refuse to read manuals.
 *              Now with 100% more hardware validation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lineblitz.h"

static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [FLAGS] <path>\n", prog_name);
    fprintf(stderr, "Flags:\n");
    fprintf(stderr, "  -f, --force    Count all files. Ignore whitelist.\n");
    fprintf(stderr, "  -s, --summary  Quiet mode. Final stats only.\n");
    fprintf(stderr, "  -b, --blitz    Enable AVX2 engine (Requires capable CPU).\n");
}

int main(int argc, char *argv[]) {
    /* Verify the user didn't forget how command lines work. */
    if (argc < 2) {
        fprintf(stderr, "Error: You need to specify a path. Telepathy is not implemented yet.\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    AppConfig config = {false, false, false, NULL};
    Stats stats = {0, 0, 0.0};

    /* Parse arguments. Manual loop because we like to see the gears turning. */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--force") == 0) {
            config.force = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--summary") == 0) {
            config.summary = true;
        } else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--blitz") == 0) {
            config.blitz = true;
        } else {
            /* First non-flag argument is the target. Others are ignored. */
            if (config.target_path == NULL) {
                config.target_path = argv[i];
            } else {
                fprintf(stderr, "Warning: Ignoring extra argument '%s'.\n", argv[i]);
            }
        }
    }

    if (config.target_path == NULL) {
        fprintf(stderr, "Error: Path missing. Where exactly are we blitzing?\n");
        return EXIT_FAILURE;
    }

    /* 
     * CRITICAL: Check if the hardware can actually handle the requested speed.
     * Better to crash politely now than to explode with a SigIll later.
     */
    enforce_hardware_requirements(&config);

    /* Start the timer. Precision matters for bragging rights. */
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* Launch the scout to find and count lines. */
    discover_and_count(&config, &stats);

    clock_gettime(CLOCK_MONOTONIC, &end);
    stats.elapsed_time_sec = (end.tv_sec - start.tv_sec) + 
                             (end.tv_nsec - start.tv_nsec) / 1e9;

    /* Print the final verdict. Try to look impressed. */
    printf("\n================ [ LineBlitz Report ] ================\n");
    printf(" Target       : %s\n", config.target_path);
    printf(" Engine       : %s\n", config.blitz ? "AVX2 (256-bit)" : "SSE2 (128-bit)");
    printf(" Files Scanned: %u\n", stats.total_files);
    printf(" Total Lines  : %llu\n", (unsigned long long)stats.total_lines);
    printf(" Elapsed Time : %.4f seconds\n", stats.elapsed_time_sec);
    printf("======================================================\n");

    return EXIT_SUCCESS;
}
