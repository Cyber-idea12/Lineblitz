/*
 * File: discovery.c
 * Description: Directory traversal and extension verification.
 *              Navigates the file system avoiding black holes like node_modules.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include "lineblitz.h"

/* The VIP list. Strict code and configs only. */
static const char *WHITELIST[] = {
    ".c", ".cpp", ".h", ".hpp", ".py", ".rs", ".go", ".java", 
    ".js", ".ts", ".json", ".sql", ".sh", ".Makefile", NULL
};

/* The garbage bins. Do not enter. */
static const char *IGNORED_DIRS[] = {
    ".git", "node_modules", "build", "obj", "bin", "__pycache__", ".idea", ".vscode", NULL
};

static bool is_ignored_dir(const char *dirname) {
    for (int i = 0; IGNORED_DIRS[i] != NULL; i++) {
        if (strcmp(dirname, IGNORED_DIRS[i]) == 0) return true;
    }
    return false;
}

static bool is_whitelisted(const char *ext) {
    if (!ext) return false;
    for (int i = 0; WHITELIST[i] != NULL; i++) {
        if (strcasecmp(ext, WHITELIST[i]) == 0) return true;
    }
    return false;
}

/* 
 * Ask the user if they really want to count non-code files.
 * Defaults to 'Yes' on simple Enter. 
 */
static bool ask_permission(const char *ext) {
    char response[10];
    printf("Found suspicious extension [%s]. Count anyway? [Y/n]: ", ext);
    
    if (fgets(response, sizeof(response), stdin) != NULL) {
        if (response[0] == 'n' || response[0] == 'N') {
            return false;
        }
    }
    /* Enter (newline) or 'y' means yes. */
    return true;
}

/* 
 * The recursive worker. Explores directories and delegates counting.
 */
static void process_path(const char *path, AppConfig *config, Stats *stats) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) return; /* File system says no. */

    if (S_ISDIR(path_stat.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) return;

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            /* Skip current and parent directories */
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            /* Skip blacklisted directories entirely */
            if (entry->d_type == DT_DIR && is_ignored_dir(entry->d_name)) {
                continue;
            }

            /* Construct new path without allocating memory dynamically if possible, 
               but for safety in deep trees, we use a fixed buffer. */
            char next_path[1024];
            snprintf(next_path, sizeof(next_path), "%s/%s", path, entry->d_name);
            process_path(next_path, config, stats);
        }
        closedir(dir);
    } 
    else if (S_ISREG(path_stat.st_mode)) {
        /* It's a regular file. Check the extension. */
        const char *dot = strrchr(path, '.');
        bool allowed = config->force;

        if (!allowed && dot) {
            if (is_whitelisted(dot)) {
                allowed = true;
            } else {
                /* Interactive prompt for unlisted extensions */
                allowed = ask_permission(dot);
            }
        }

        if (allowed) {
            uint64_t lines = config->blitz ? count_lines_avx2(path) : count_lines_sse(path);
            
            if (lines != (uint64_t)-1) {
                stats->total_lines += lines;
                stats->total_files++;
                
                if (!config->summary) {
                    printf("%-50s : %llu lines\n", path, (unsigned long long)lines);
                }
            }
        }
    }
}

void discover_and_count(AppConfig *config, Stats *stats) {
    /* Kick off the recursive traversal from the target path. */
    process_path(config->target_path, config, stats);
}
