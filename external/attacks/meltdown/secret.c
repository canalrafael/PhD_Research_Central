#include "libkdump.h"
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>

const char *strings[] = {
    "If you can read this, this is really bad",
    "Burn after reading this string, it is a secret string",
    "Congratulations, you just spied on an application",
    "Wow, you broke the security boundary between user space and kernel",
    "Welcome to the wonderful world of microarchitectural attacks",
    "Please wait while we steal your secrets...",
    "Don't panic... But your CPU is broken and your data is not safe",
    "How can you read this? You should not read this!"};

int main(int argc, char *argv[]) {
    // 1. Initialize configuration
    libkdump_config_t config;
    memset(&config, 0, sizeof(libkdump_config_t));

    // Default ARMv8 settings for your FZ3
    config.cache_miss_threshold = 100;
    config.measurements = 10;
    config.physical_offset = 0x80000000; 

    libkdump_init(config);

    // 2. Select the secret string
    srand(time(NULL));
    const char *secret = strings[rand() % (sizeof(strings) / sizeof(strings[0]))];
    int len = strlen(secret);

    // 3. Pin memory to prevent it from moving (Critical for Meltdown)
    if (mlock(secret, len) != 0) {
        perror("\x1b[31;1m[!]\x1b[0m mlock failed (try running with sudo)");
    }

    // 4. Print address information for your research
    printf("\x1b[32;1m[+]\x1b[0m Secret: \x1b[33;1m%s\x1b[0m\n", secret);
    printf("\x1b[32;1m[+]\x1b[0m Virtual address of secret:  \x1b[32;1m0x%zx\x1b[0m\n", (size_t)secret);

    size_t paddr = libkdump_virt_to_phys((size_t)secret);
    if (!paddr) {
        printf("\x1b[31;1m[!]\x1b[0m Program requires root privileges to resolve physical addresses!\n");
        libkdump_cleanup();
        exit(1);
    }

    printf("\x1b[32;1m[+]\x1b[0m Physical address of secret: \x1b[32;1m0x%zx\x1b[0m\n", paddr);
    printf("\x1b[32;1m[+]\x1b[0m Exit with \x1b[37;1mCtrl+C\x1b[0m if you are done\n");

    // 5. Activity loop to keep the secret in the L1 Cache
    while (1) {
        volatile size_t dummy = 0;
        for (int i = 0; i < len; i++) {
            dummy += secret[i]; // Constantly access the secret
        }
        sched_yield(); // Give the reader process time to run on the core
    }

    libkdump_cleanup();
    return 0;
}