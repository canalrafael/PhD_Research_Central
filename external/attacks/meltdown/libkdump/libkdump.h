#ifndef _LIBKDUMP_H_
#define _LIBKDUMP_H_

#include <stdint.h>
#include <stdio.h>

// Bypassed x86 guard for ARMv8-A research
// #if !(defined(__x86_64__) || defined(__i386__))
// # error x86-64 and i386 are the only supported architectures
// #endif

// Standard Cache Line Size for Cortex-A53
#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#endif

// Default physical offset for FZ3 PetaLinux (verify with your kernel module)
#define DEFAULT_PHYSICAL_OFFSET 0x80000000ull

typedef enum { SIGNAL_HANDLER, TSX } libkdump_fault_handling_t;
typedef enum { NOP, IO, YIELD } libkdump_load_t;

typedef struct {
  size_t cache_miss_threshold;
  libkdump_fault_handling_t fault_handling;
  int measurements;
  int accept_after;
  int load_threads;
  libkdump_load_t load_type;
  int retries;
  size_t physical_offset;
} libkdump_config_t;

int libkdump_init(const libkdump_config_t configuration);
int libkdump_read(size_t addr);
void libkdump_cleanup();
size_t libkdump_virt_to_phys(size_t addr);
size_t libkdump_phys_to_virt(size_t addr);

#endif