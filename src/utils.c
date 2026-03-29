/*
 * File: utils.c
 * Description: Hardware interrogation. 
 *              Because users will absolutely try to run AVX2 on a toaster.
 */

#include <stdio.h>
#include <stdlib.h>
#include <cpuid.h>
#include "lineblitz.h"

/* 
 * Queries the CPU at runtime to check if AVX2 is actually supported.
 */
static bool check_avx2_support(void) {
    unsigned int eax, ebx, ecx, edx;
    
    /* CPUID leaf 7, sub-leaf 0 contains the AVX2 bit in EBX (bit 5) */
    if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
        if ((ebx & (1 << 5)) != 0) {
            return true;
        }
    }
    return false;
}

/* 
 * Call this early in main.c to prevent explosive failures.
 */
void enforce_hardware_requirements(AppConfig *config) {
    if (config->blitz && !check_avx2_support()) {
        fprintf(stderr, "\n[FATAL ERROR]\n");
        fprintf(stderr, "You requested --blitz, but your silicon was baked before AVX2 existed.\n");
        fprintf(stderr, "Downgrade your expectations or upgrade your CPU.\n");
        exit(EXIT_FAILURE);
    }
}
